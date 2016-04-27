/*
 * Simulator`.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#include "Simulator.h"
#include <algorithm>
#include <sstream>
#include <dirent.h>
#include <cstdio>
#include <dlfcn.h>
#include <list>
#include <map>
#include <sys/stat.h>
#include <sys/stat.h>
#include "Algorithms/ExternalAlgo.h"
#include <iomanip>

using namespace std;

// our global factory for making Algos
map<string, maker_t*> factory;
vector<string> algosNames;
vector<string> Logger::vLog;

/* Returns a list of files in a directory */
int GetFilesInDirectory(std::vector<string> &out, const string &directory)
{
    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(directory.c_str());
    if(dir == NULL)
    	return -1;

    while ((ent = readdir(dir)) != NULL) {
        const string file_name = ent->d_name;
        const string full_file_name = directory + (directory[directory.length() - 1] == '/' ? "" : "/") + file_name;

        if (stat(full_file_name.c_str(), &st) == -1)
            continue;

        const bool is_directory = (st.st_mode & S_IFDIR) != 0;

        if (is_directory)
            continue;

        out.push_back(full_file_name);
    }
    if(closedir(dir) == -1)
    	return -1;

    return 0;
}


void GetFilesListWithSuffix(const string &sPath, const string &sSuffix, vector<string> &vDirTypeFiles)
{
	// find house files
	vector<string> vDirFiles;
	GetFilesInDirectory(vDirFiles, sPath);

	for(auto oFileIter = vDirFiles.cbegin(); oFileIter != vDirFiles.cend(); oFileIter++)
	{
		size_t nPos = oFileIter->find_last_of(".");
		if(nPos != string::npos && oFileIter->substr(nPos + 1) == sSuffix)
			vDirTypeFiles.push_back(*oFileIter);
	}
}

string getFileNameFromPath(const string &sPath, bool bWithExtension)
{
    size_t start = sPath.find_last_of(PATH_SEPARATOR) + 1;

    size_t end;
    if(!bWithExtension)
    	end = sPath.find_last_of('.');
    else
    	end = sPath.length();

    return sPath.substr(start, end - start);
}

string getFolderPath(const string &sFilePath)
{
    return sFilePath.substr(0, sFilePath.find_last_of(PATH_SEPARATOR));
}

int LoadAlgoFilesToFactory(const vector<string> &vAlgoFilesPaths)
{
    int nErrorCount = 0;

    if(vAlgoFilesPaths.size() == 0)
		throw InnerException("no algo file in path");

	void *pDlib;
	for (const string& sAlgoPath : vAlgoFilesPaths)
	{
		size_t nOldFactorySize = factory.size();

		pDlib = dlopen(sAlgoPath.c_str(), RTLD_NOW);
		if (pDlib == nullptr)
		{
			cout << dlerror() << endl;
            string strError = getFileNameFromPath(sAlgoPath, true) + ": file cannot be loaded or is not a valid .so";
			Logger::addLogMSG(strError);
			nErrorCount++;
		}
		else
        {
			if(nOldFactorySize == factory.size())
			{
	            string strError = getFileNameFromPath(sAlgoPath, true) + ": valid .so but no algorithm was registered after loading it";
				Logger::addLogMSG(strError);
				nErrorCount++;
			}
			else
			{
				size_t start = sAlgoPath.find_last_of(PATH_SEPARATOR) + 1;
				size_t end = sAlgoPath.find_last_of('.');

				algosNames.push_back(sAlgoPath.substr(start, end - start));
			}
		}
	}

	if(nErrorCount == (int)vAlgoFilesPaths.size())
	{
        string path = vAlgoFilesPaths[0];
        string strError = "All algorithm files in target folder '" + getFolderPath(path) + "' cannot be opened or are invalid:";
		throw  InnerException(strError);
	}

	return 0;
}

Simulator::Simulator(const string &sConfigFilePath, const string &sHousesPath , const string &sAlgosPath)
{

    vector<string> vDirAlgosFiles;

	//Read Configuration File to members
	ReadConfig(sConfigFilePath);

	// Load houses
	LoadHouses(sHousesPath);

	LoadAlgos(vDirAlgosFiles, sAlgosPath);
	LoadAlgoFilesToFactory(vDirAlgosFiles);
}

// Reads configuration file and sets m_config keys
void Simulator::ReadConfig(const string &sConfigFilePath)
{
	string line;
    struct stat buf;

    if (stat(sConfigFilePath.c_str(), &buf) == -1){
        string strError = "config.ini doesn't exists in '" + sConfigFilePath + "'";
        throw  InnerException(strError);
    }

    ifstream fin(sConfigFilePath);
	//check if file open with path, if not try open file in current dir
	if (!fin)
	{
		string strError = "config.ini exists in ' " + sConfigFilePath + "' but cannot be opened";
		throw  InnerException(strError);
	}

	while (getline(fin, line))
	{
        if(line.length() > 0) {
            stringstream ss(line);
            string item;
            vector<string> tokens;
            while (getline(ss, item, '=')) {
                tokens.push_back(item);
            }

            m_config[trim(tokens[0])] = stoi(trim(tokens[1]));
		}
	}

    int nMissingCount = 0;
    string strMissingParams = "";

	//Checking all config paramters
	if (m_config.find(BATTERY_CAPACITY_KEY) == m_config.end()){
        nMissingCount++;
        strMissingParams += BATTERY_CAPACITY_KEY;
	}

	if (m_config.find(BATTERY_CONSUMPTION_KEY) == m_config.end()){
        if(nMissingCount != 0)
        {
            strMissingParams += ", ";
        }

        nMissingCount++;
        strMissingParams += BATTERY_CONSUMPTION_KEY;
	}

	if (m_config.find(BATTERY_RECHARGE_KEY) == m_config.end()){
	        if(nMissingCount != 0)
        {
            strMissingParams += ", ";
        }

        nMissingCount++;
        strMissingParams += BATTERY_RECHARGE_KEY;
	}

	if (m_config.find(MAX_STEPS_AFTER_KEY) == m_config.end()){
	        if(nMissingCount != 0)
        {
            strMissingParams += ", ";
        }

        nMissingCount++;
        strMissingParams += MAX_STEPS_AFTER_KEY;
	}

	if(nMissingCount != 0)
	{
        string strError = std::string("config.ini missing ") + std::to_string(nMissingCount) + std::string(" parameter(s) : ") + strMissingParams;
        throw InnerException(strError);
	}
}

// Initializes houses (In exercise 1 there is only 1 hard-coded house)
void Simulator::LoadHouses(const string &sHousesPath)
{
    vector<string> vDirHousesFiles;
    GetFilesListWithSuffix(sHousesPath, "house", vDirHousesFiles);

	// load houses
	for(string &sHouse : vDirHousesFiles){
        string name = sHouse;
        size_t start = name.find_last_of(PATH_SEPARATOR) + 1;
        name = name.substr(start, name.length());
        size_t endPoint = name.find_last_of('.');
        name = name.substr(0, endPoint);
		m_vOriginalHouses.push_back(new House(name, sHouse, m_config[BATTERY_CAPACITY_KEY], m_config[BATTERY_CONSUMPTION_KEY], m_config[BATTERY_RECHARGE_KEY]));
    }
}

void Simulator::LoadAlgos(std::vector<string> &vDirAlgosFilesOut, const string &sAlgosPath)
{
	// find house files
    GetFilesListWithSuffix(sAlgosPath, "so", vDirAlgosFilesOut);
}

// Reloads simulations
void Simulator::ReloadSimulations(House *oHouse)
{
	for(OneSimulation *pSim : m_vSimulations)
		delete pSim;

	m_vSimulations.clear();
    int i = 0;
	for(AbstractAlgorithm *pAlgo : m_vAlgos){
		m_vSimulations.push_back(new Simulator::OneSimulation(*oHouse, pAlgo, m_config, algosNames.at(i)));
		i++;
    }
}

// Reloads algorithms
void Simulator::ReloadAlgorithms()
{
	for(AbstractAlgorithm *pAlgo : m_vAlgos)
		delete pAlgo;

	m_vAlgos.clear();

	for(auto itr = factory.begin(); itr != factory.end(); itr++)
	{
        m_vAlgos.push_back((itr->second)());
   	}
}

// Runs the simulation
void Simulator::Run()
{
    map<string, map<string, int>> oScores;
	// For every house ran all simulations in parallel
	for(House *pHouse : m_vOriginalHouses)
	{
        map<string, int> houseScore;
		ReloadAlgorithms();
		ReloadSimulations(pHouse);

		bool bSomeActive = true;
		bool bIsWinner = false;
		bool bAnnounceWinner = false;
		int nSimulationSteps = 0;
		int nMaxSimulationSteps = pHouse->GetMaxSteps(); // Get From House
		int nWinnerSteps = 0;
		OneSimulation* lastFinnished = nullptr;
		int nFinishedCount = 0;

		// Run until some algorithms didn't finish and simulation maximum steps count was not reached
		while(bSomeActive
					&& nSimulationSteps < nMaxSimulationSteps
					&& (!bIsWinner || nSimulationSteps < nWinnerSteps + m_config[MAX_STEPS_AFTER_KEY]))
		{
			bSomeActive = false;
			for(OneSimulation *oSim : m_vSimulations)
			{

				if(oSim->GetSimulationState() == OneSimulation::Running)
				{
					bSomeActive = true;

					// When there is a winner or steps == MaxSteps - MaxStepsAfterWinner algorithm should receive AboutToFinish announcement
					if(bAnnounceWinner || (nSimulationSteps == (nMaxSimulationSteps - m_config[MAX_STEPS_AFTER_KEY])))
					{
						oSim->AnnounceAboutToFinish();
					}

					// Make a single simulation step
					oSim->MakeStep();

					if(oSim->GetSimulationState() == OneSimulation::Finished)
					{
						// calculate Actual Position In Competition according to forum post by Amir
						if (lastFinnished != nullptr) // someone already won
						{
							if (lastFinnished->getSteps() == oSim->getSteps()) // someone finished on the same step
								oSim->SetActualPositionInCompetition(lastFinnished->GetActualPositionInCompetition());  // set the same ActualPositionInCompetition
							else
								oSim->SetActualPositionInCompetition(nFinishedCount + 1);
						}
						else // first to finish
						{
							oSim->SetActualPositionInCompetition(1);
						}

						lastFinnished = oSim;

						// if there was no winner until now: save winner steps and remember to announce winner at the beginning of the next round
						if(!bIsWinner)
						{
							bIsWinner = true;
							nWinnerSteps = oSim->getSteps();
							bAnnounceWinner = true; 		// remember to announce winner at the beginning of the next round
						}

						nFinishedCount++;
					}
				}
			}

			nSimulationSteps++;
		}

		// calculate and print score
		for(OneSimulation *oSim : m_vSimulations)
		{
			int nScore = oSim->CalculateScore(nWinnerSteps, bIsWinner, nSimulationSteps);

			oScores[oSim->getAlgoFileName()][pHouse->GetHouseFileName()] = nScore;
		}
	}


	int nameWidth = 13;
	int scoreWidth = 10;
	const char seperator = '|';
	const char space = ' ';
	const char lineSep = '-';

	// print scores table
    int nLineN = nameWidth + 2 + (m_vOriginalHouses.size() + 1) * (scoreWidth + 1);

    cout << setw(nLineN) << setfill(lineSep)<<  lineSep << endl;

    cout << seperator << setw(nameWidth) << setfill(space) << "" << seperator;

    // print house names
	for(const auto &oHousesScoresPair : oScores[algosNames[0]])
	{
        cout << left << setw(scoreWidth) << setfill(space) << oHousesScoresPair.first << seperator;
	}

	cout << left << setw(scoreWidth) << setfill(space) << "AVG" << seperator << endl;

    cout << setw(nLineN) << setfill(lineSep)<<  lineSep << endl;
    // for each algorithm: print scores
    for(const auto &oAlgoHousesPair: oScores)
    {
    	cout << seperator << left << setw(nameWidth) << setfill(space) << oAlgoHousesPair.first << seperator;

        int nCount = 0;
        int nSum = 0;

        // for every house that algorithm ran on: print the score
    	for(const auto &oHousesScoresPair : oAlgoHousesPair.second)
    	{
            cout << right << setw(scoreWidth) << setfill(space) << oHousesScoresPair.second << seperator;

            nCount++;
            nSum += oHousesScoresPair.second;
    	}

        double dAvg = nSum / (double)nCount;
        cout << setw(scoreWidth) << setfill(space) << fixed << setprecision(2) << dAvg << seperator<< endl;

        cout << setw(nLineN) << setfill(lineSep)<<  lineSep << endl;
    }
}

Simulator::~Simulator()
{
	for(OneSimulation *pSim : m_vSimulations)
		delete pSim;

	for(AbstractAlgorithm *pAlgo : m_vAlgos)
		delete pAlgo;

	for(House *pHouse : m_vOriginalHouses)
		delete pHouse;
}

// Make single simulation step
void Simulator::OneSimulation::MakeStep()
{
	Direction oDir = m_pAlgo->step();

	const int i = m_oHouse.GetVacuumPos().i;
	const int j = m_oHouse.GetVacuumPos().j;

	// spot illegal move
	if(		(oDir == Direction::North 	&& (m_oHouse.GetVacuumPos().i == 0 								|| m_oHouse[i - 1][j    ] == WALL_CELL))
		||	(oDir == Direction::East 	&& (m_oHouse.GetVacuumPos().j == m_oHouse.GetColNumber() - 1	|| m_oHouse[i    ][j + 1] == WALL_CELL))
		||	(oDir == Direction::South 	&& (m_oHouse.GetVacuumPos().i == m_oHouse.GetRowNumber() - 1	|| m_oHouse[i + 1][j    ] == WALL_CELL))
		||	(oDir == Direction::West 	&& (m_oHouse.GetVacuumPos().j == 0						 		|| m_oHouse[i    ][j - 1] == WALL_CELL)))
	{
		SimulationState = AlgoMadeIllegalMove;
		return;
	}

	// update vacuum position
	m_oHouse.TryMoveVacuum(oDir);

	m_nSteps++;

	// check battery level
	if (m_oHouse.GetBatteryLevel() == 0 && m_oHouse[m_oHouse.GetVacuumPos().i][m_oHouse.GetVacuumPos().j] != DOCKING_STATION_CELL)
	{
		SimulationState = OutOfBattery;
		return;
	}

	// if all dirt collected and back to docking station mark as finished
	if(m_oHouse.GetDirtCollected() == m_oHouse.GetInitialAmounthOfDirt() && m_oHouse[m_oHouse.GetVacuumPos().i][m_oHouse.GetVacuumPos().j] == DOCKING_STATION_CELL)
		SimulationState = Finished;
}

// Calculate score
int Simulator::OneSimulation::CalculateScore(int nWinnerSteps, bool bIsWinner, int nSimulationSteps) const
{
	int nScore;
	if (SimulationState == OneSimulation::AlgoMadeIllegalMove)
		nScore = 0;
	else
	{
		// calculate position in competion
		int nPositionInCompetition;
		if (SimulationState == OneSimulation::Finished)
			nPositionInCompetition = std::min(4, m_nActualPositionInCompetition);
		else
			nPositionInCompetition = 10;

		// if no one won set nWinnerSteps to nSimulationSteps
		if (!bIsWinner)
			nWinnerSteps = nSimulationSteps;

		// calculate this simulation steps
		int nThisSimulationSteps;
		if (SimulationState == OneSimulation::OutOfBattery)
			nThisSimulationSteps = nSimulationSteps;
		else
			nThisSimulationSteps = m_nSteps;

		// check wether the vacuum is in docking station
		bool bIsBackToDockingStation = m_oHouse[m_oHouse.GetVacuumPos().i][m_oHouse.GetVacuumPos().j] == DOCKING_STATION_CELL;

		nScore = std::max(0,
			2000
			- (nPositionInCompetition - 1) * 50
			+ (nWinnerSteps - nThisSimulationSteps) * 10
			- (m_oHouse.GetInitialAmounthOfDirt() - m_oHouse.GetDirtCollected()) * 3
			+ (bIsBackToDockingStation ? 50 : -200));
	}

	return nScore;
}

static string trim(string& str)
{
	str.erase(0, str.find_first_not_of(' '));
	str.erase(str.find_last_not_of(' ') + 1);
	return str;
}

// MAIN
int main(int argsc, char **argv)
{
	int i;
	string sConfigPath = "";
	string sHousesPath = "";
	string sAlgosPath = "";

	// Gets Command line parameters
	for (i = 1; i < argsc; i++)
	{
		if (CONFIG_PATH_FLAG.compare(argv[i]) == 0)
		{
			if (i < (argsc - 1))
				sConfigPath = argv[++i];
		}
		else if (HOUSE_PATH_FLAG.compare(argv[i]) == 0)
		{
			if (i < (argsc - 1))
				sHousesPath = argv[++i];
		}
		else if (ALGO_PATH_FLAG.compare(argv[i]) == 0)
		{
			if (i < (argsc - 1))
				sAlgosPath = argv[++i];
		}
		else
		{
			cout << "Usage: simulator [-config <config_file_location >] [-house_path <houses_path_location>] [algorithm_path <algorithm path>]" << endl;
			return 1;
		}
	}

    if(sConfigPath.length() != 0)
    {
        sConfigPath = sConfigPath.length() == 0 ? "." : sConfigPath;
        // Add config Path dir sign id needed
        if (sConfigPath[sConfigPath.length() - 1] != PATH_SEPARATOR)
            sConfigPath += PATH_SEPARATOR;
    }
    else
    {
        sConfigPath = "./";
    }

	// Concat file name
	sConfigPath += CONFIG_FILE_NAME;

	sHousesPath = sHousesPath.length() == 0 ? "." : sHousesPath;
	// Add Houses Path dir sign id needed
	if (sHousesPath[sHousesPath.length() - 1] != PATH_SEPARATOR)
		sHousesPath += PATH_SEPARATOR;

	sAlgosPath = sAlgosPath.length() == 0 ? "." : sAlgosPath;
	// Add Algos Path dir sign id needed
	if (sAlgosPath[sAlgosPath.length() - 1] != PATH_SEPARATOR)
		sAlgosPath += PATH_SEPARATOR;

	try
	{
		Simulator sim(sConfigPath, sHousesPath, sAlgosPath);
		sim.Run();
	}
	catch (exception& ex)
	{
		cout << ex.what() << endl;
	}

	if(Logger::getLog().size() != 0)
		cout << endl <<"Errors:" << endl;

	for(const string &sLogEntry : Logger::getLog())
	{
        cout << sLogEntry << endl;
	}

	return 0;
}
