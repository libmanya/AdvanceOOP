/*
 * Simulator.cpp
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
#include <iomanip>
#include <stdlib.h>
#include <thread>
#include <mutex>
#include "AlgorithmRegistrar.h"
#include "makeUnique.h"
#include "AlgorithmRegistration.h"
#include "Encoder.h"

using namespace std;

bool bDebug = false;

vector<string> Logger::vHousesLog;
vector<string> Logger::vAlgosLog;
vector<string> Logger::vScoresLog;
vector<string> Logger::vVideoLog;
AlgorithmRegistrar AlgorithmRegistrar::instance;

typedef void * __attribute__ ((__may_alias__)) pvoid_may_alias_t;

AlgorithmRegistration::AlgorithmRegistration(std::function<unique_ptr<AbstractAlgorithm>()> algorithmFactory) {
    AlgorithmRegistrar::getInstance().registerAlgorithm(algorithmFactory);
}

/* Returns a list of files in a directory */
int GetFilesInDirectory(std::vector<string> &out, const string &directory)
{
    DIR *dir;
    class dirent *ent;
    class stat st;

    dir = opendir(directory.c_str());
    if(dir == NULL)
        return -1;

    while ((ent = readdir(dir)) != NULL)
    {
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

int GetFilesListWithSuffix(const string &sPath, const string &sSuffix, vector<string> &vDirTypeFiles)
{
    // find house files
    vector<string> vDirFiles;
    int nRc = GetFilesInDirectory(vDirFiles, sPath);

    if(nRc == -1)
        return -1;

    for(auto oFileIter = vDirFiles.cbegin(); oFileIter != vDirFiles.cend(); oFileIter++)
    {
    	size_t nSuffixLen = sSuffix.length();

    	if(oFileIter->length() >= nSuffixLen)
    	{
    		if(oFileIter->substr(oFileIter->length() - nSuffixLen, nSuffixLen) == sSuffix)
                vDirTypeFiles.push_back(*oFileIter);
    	}

        //size_t nPos = oFileIter->find_last_of(".");
        //if(nPos != string::npos && oFileIter->substr(nPos + 1) == sSuffix)
         //   vDirTypeFiles.push_back(*oFileIter);
    }

    return 0;
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

// constructs full path from relative path
string GetFullPath(const string& sRelativePath)
{
    char pActualpath[PATH_MAX+1];
    char *ptr;

    ptr = realpath(sRelativePath.c_str(), pActualpath);

    if( ptr == nullptr )
    	throw InnerException("Path doesn't exist");

    return string(ptr);
}

int AlgorithmRegistrar::loadAlgorithm(const std::string& so_file_name) {
    size_t size = instance.size();

    void *pDlib;
    pDlib = dlopen(so_file_name.c_str(), RTLD_NOW);
    if (pDlib == nullptr)
    {
        string strError = getFileNameFromPath(so_file_name, true) + ": file cannot be loaded or is not a valid .so";
        Logger::addLogMSG(strError, Logger::LogType::Algos);
        return FILE_CANNOT_BE_LOADED;
    }

    m_vAlgoLibHandles.push_back(pDlib);

    if(instance.size() == size) {
        string strError = getFileNameFromPath(so_file_name, true) + ": valid .so but no algorithm was registered after loading it";
        Logger::addLogMSG(strError, Logger::LogType::Algos);
        return NO_ALGORITHM_REGISTERED;
    }

    instance.setNameForLastAlgorithm(so_file_name);


    return ALGORITHM_REGISTERED_SUCCESSFULY;
}

int Simulator::LoadAlgoFilesToFactory(const vector<string> &vAlgoFilesPaths)
{
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();

    for(const auto& algorithmSoFileName : vAlgoFilesPaths) {
        registrar.loadAlgorithm(algorithmSoFileName);
    }

    if(0 == (int) registrar.size())
    {
        string path = vAlgoFilesPaths[0];
        cout << "All algorithm files in target folder '"
             << GetFullPath(getFolderPath(path))
             << "' cannot be opened or are invalid:"
             << endl;

        for(const string &sLogEntry : Logger::getLog(Logger::LogType::Algos, true))
            cout << sLogEntry << endl;

        throw  InnerException();
    }

    return 0;
}

Simulator::Simulator(const string &sConfigFilePath, const string &sHousesPath , const string &sAlgosPath, const string &scorePath, int numOfThreads, bool bCreateVideo)
{

    vector<string> vDirAlgosFiles;

    //Read Configuration File to members
    ReadConfig(sConfigFilePath);

    //Get Score Function
    LoadScoreFile(scorePath);

    GetSOFiles(vDirAlgosFiles, sAlgosPath);
    if(vDirAlgosFiles.size() == 0)
    {
        string sMsg = USAGE + "\n" + "cannot find algorithm files in '" + GetFullPath(sAlgosPath) + "'";
        throw InnerException(sMsg);
    }
    LoadAlgoFilesToFactory(vDirAlgosFiles);

    // Load houses
    LoadHouses(sHousesPath);

    //check valid number of thread - initialize to 1
    if(numOfThreads > 1)
    {
        if (numOfThreads > (int)m_vOriginalHouses.size())
        {
            m_nNumOfThreads = m_vOriginalHouses.size();
        }
        else
        {
            m_nNumOfThreads = numOfThreads;
        }
    }

    m_bCreateVideo = bCreateVideo;
}

// Reads configuration file and sets m_config keys
void Simulator::ReadConfig(const string &sConfigFilePath)
{
    string line;
    struct stat buf;

    if (stat(sConfigFilePath.c_str(), &buf) == -1)
    {
    	string sMsg = "cannot find config.ini file in '" + GetFullPath(getFolderPath(sConfigFilePath)) + "'";
        throw  InnerException(USAGE + "\n" + sMsg);
    }

    ifstream fin(sConfigFilePath);
    //check if file open with path, if not try open file in current dir
    if (!fin)
    {
        string strError = "config.ini exists in '" + GetFullPath(getFolderPath(sConfigFilePath)) + "' but cannot be opened";
        throw  InnerException(strError);
    }

    int nBadCount = 0;
    string strBadParams = "";

    while (getline(fin, line))
    {
        if(line.length() > 0)
        {
            stringstream ss(line);
            string item;
            vector<string> tokens;
            while (getline(ss, item, '='))
            {
                tokens.push_back(item);
            }

            string temp = tokens[1];
            int nParam;
            string strKey;

            if(temp.length() > 0){
                nParam = atoi(trim(tokens[1]).c_str());
                strKey = trim(tokens[0]);
                m_config[strKey] = nParam;
            }
            else
            {
                nParam = 0;
            }

            if((nParam <= 0) &&
                ((strKey.compare(BATTERY_CAPACITY_KEY) == 0) ||
                 (strKey.compare(BATTERY_CONSUMPTION_KEY) == 0) ||
                 (strKey.compare(BATTERY_RECHARGE_KEY) == 0) ||
                 (strKey.compare(MAX_STEPS_AFTER_KEY) == 0)))
            {
            	if(nBadCount != 0)
            		strBadParams += ", ";

            	nBadCount++;
            	strBadParams += trim(tokens[0]);
            }
        }
    }

    int nMissingCount = 0;
    string strMissingParams = "";

    //Checking all config paramters
    if (m_config.find(BATTERY_CAPACITY_KEY) == m_config.end())
    {
        nMissingCount++;
        strMissingParams += BATTERY_CAPACITY_KEY;
    }

    if (m_config.find(BATTERY_CONSUMPTION_KEY) == m_config.end())
    {
        if(nMissingCount != 0)
            strMissingParams += ", ";

        nMissingCount++;
        strMissingParams += BATTERY_CONSUMPTION_KEY;
    }

    if (m_config.find(BATTERY_RECHARGE_KEY) == m_config.end())
    {
        if(nMissingCount != 0)
            strMissingParams += ", ";

        nMissingCount++;
        strMissingParams += BATTERY_RECHARGE_KEY;
    }

    if (m_config.find(MAX_STEPS_AFTER_KEY) == m_config.end())
    {
        if(nMissingCount != 0)
            strMissingParams += ", ";

        nMissingCount++;
        strMissingParams += MAX_STEPS_AFTER_KEY;
    }

    if(nMissingCount != 0)
    {
        string strError = std::string("config.ini missing ") + std::to_string(nMissingCount) + std::string(" parameter(s): ") + strMissingParams;

        if(nBadCount != 0)
        	strError += "\n" + string("config.ini having bad values for ") + to_string(nBadCount) + std::string(" parameter(s): ") + strBadParams;


        throw InnerException(strError);
    }

    if(nBadCount != 0)
    {
        	string strError = "\n" + string("config.ini having bad values for ") + to_string(nBadCount) + std::string(" parameter(s): ") + strBadParams;
        	throw InnerException(strError);
    }
}

void Simulator::LoadScoreFile(const string &sScoreFilePath)
{
    if(sScoreFilePath.length() != 0)
    {
        string sScorePath = sScoreFilePath + SCORE_FILE_NAME;

        //load so score function
        struct stat buf;

        if (stat(sScorePath.c_str(), &buf) == -1)
        {
            cout << USAGE << endl;
            string strError = "cannot find " + SCORE_FILE_NAME + " file in '" + GetFullPath(getFolderPath(sScoreFilePath)) +"'";
            throw  InnerException(strError);
        }

        void *pDlib = dlopen(sScorePath.c_str(), RTLD_NOW);
        if (pDlib == nullptr)
        {
            string strError = SCORE_FILE_NAME + " exists in '" + GetFullPath(getFolderPath(sScoreFilePath)) +"' but cannot be opened or is not a valid .so";
            throw  InnerException(strError);
        }
        else
        {
            dlerror();

	    *(pvoid_may_alias_t *)(&calc_score) = dlsym(pDlib, "calc_score");
            const char *dlsym_error = dlerror();
            if (dlsym_error) {
                string strError = SCORE_FILE_NAME + " is a valid .so but it does not have a valid score formula";
                dlclose(pDlib);
                throw  InnerException(strError);
            }

            m_bIsDefaultScore = false;
            m_ptrScoreHandle = pDlib;
        }
    }
}

// Initializes houses
void Simulator::LoadHouses(const string &sHousesPath)
{
    vector<string> vDirHousesFiles;
    int nRC = GetFilesListWithSuffix(sHousesPath, "house", vDirHousesFiles);

    if (nRC == -1 || vDirHousesFiles.size() == 0)
    {
        string sMsg = USAGE + "\n" + "cannot find house files in '" + GetFullPath(sHousesPath) + "'";
        throw InnerException(sMsg);
    }

    // load houses
    for(string &sHouse : vDirHousesFiles)
    {
        string sHouseFileName = getFileNameFromPath(sHouse, false);

        unique_ptr<House> pTempHouse = make_unique<House>(std::move(sHouseFileName), sHouse, m_config[BATTERY_CAPACITY_KEY], m_config[BATTERY_CONSUMPTION_KEY], m_config[BATTERY_RECHARGE_KEY]);

        if (!pTempHouse->isLoadFailed())
            m_vOriginalHouses.push_back(std::move(pTempHouse));
    }

    if (m_vOriginalHouses.size() == 0)
    {
        cout << "All house files in target folder '"
             << GetFullPath(sHousesPath)
             << "' cannot be opened or are invalid:"
             << endl;

        for(const string &sLogEntry : Logger::getLog(Logger::LogType::Houses, true))
            cout << sLogEntry << endl;

        throw InnerException();
    }
}

void Simulator::GetSOFiles(std::vector<string> &vDirAlgosFilesOut, const string &sAlgosPath)
{
    // find house files
    GetFilesListWithSuffix(sAlgosPath, ALGO_FILE_SUFFIX, vDirAlgosFilesOut);
}

void Simulator::loadAlgorithms(vector<unique_ptr<AbstractAlgorithm>>& Algos)
{
    AlgorithmRegistrar& registrar = AlgorithmRegistrar::getInstance();
    auto algorithms = registrar.getAlgorithms();
    auto algorithmsNames = registrar.getAlgorithmNames();
    auto pName = algorithmsNames.begin();

    for(auto& algorithm: algorithms)
    {
        Algos.push_back(std::move(algorithm));
        string sAlgoPath = *pName;
        size_t start = sAlgoPath.find_last_of(PATH_SEPARATOR) + 1;
        size_t end = sAlgoPath.find_last_of('.');
        m_vAlgoFileNames.push_back(sAlgoPath.substr(start, end - start));
        pName++;
    }
}

void Simulator::loadSimulations(unique_ptr<House> &oHouse, vector<unique_ptr<OneSimulation>> &vSimulations, vector<unique_ptr<AbstractAlgorithm>>& Algos)
{
    int i = 0;
    for(unique_ptr<AbstractAlgorithm>& oAlgo : Algos)
    {
        vSimulations.push_back(make_unique<OneSimulation>(*oHouse, std::move(oAlgo), m_config, m_vAlgoFileNames[i]));
        i++;
    }
}

void Simulator::RunOnHouseThread()
{
    while (!m_HouseQueue.isEmpty())
    {
        unique_ptr<House> pHouse = m_HouseQueue.pop();

        if(pHouse != nullptr)
        {
            vector<unique_ptr<OneSimulation>> vSimulations;
            vector<unique_ptr<AbstractAlgorithm>> Algos;

            loadAlgorithms(Algos);
            loadSimulations(pHouse, vSimulations, Algos);

            bool bSomeActive = true;
            bool bIsWinner = false;
            bool bAnnounceWinner = false;
            int nSimulationSteps = 0;
            int nMaxSimulationSteps = pHouse->GetMaxSteps(); // Get From House
            int nWinnerSteps = 0;
            int nlastFinnished = -1;
            int nFinishedCount = 0;

                    // Run until some algorithms didn't finish and simulation maximum steps count was not reached
            while(bSomeActive
                    && nSimulationSteps < nMaxSimulationSteps
                    && (!bIsWinner || nSimulationSteps < nWinnerSteps + m_config[MAX_STEPS_AFTER_KEY]))
            {
                bSomeActive = false;
                for(size_t i = 0; i < vSimulations.size(); i++)
                {
                	unique_ptr<OneSimulation> &oSim = vSimulations[i];

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

                        if(m_bCreateVideo)
                        {
                            oSim->montageHouse();
                        }

                        if(oSim->GetSimulationState() == OneSimulation::Finished)
                        {
                            // calculate Actual Position In Competition according to forum post by Amir
                            if (nlastFinnished != -1) // someone already won
                            {
                                if (vSimulations[nlastFinnished]->getSteps() == oSim->getSteps()) // someone finished on the same step
                                    oSim->SetActualPositionInCompetition(vSimulations[nlastFinnished]->GetActualPositionInCompetition());  // set the same ActualPositionInCompetition
                                else
                                    oSim->SetActualPositionInCompetition(nFinishedCount + 1);
                            }
                            else // first to finish
                            {
                                oSim->SetActualPositionInCompetition(1);
                            }

                            nlastFinnished = (int)i;

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

            if(m_bCreateVideo)
            {
                for(size_t i = 0; i < vSimulations.size(); i++)
                {
                    unique_ptr<OneSimulation> &oSim = vSimulations[i];
                    int nErrors = oSim->getNumberOfFailedMontage();

                    if(nErrors > 0)
                    {
                        string error = "Error: In the simulation " + oSim->getAlgoFileName() + " ," + oSim->getHouseFileName() + ": the creation of " + to_string(nErrors) + " images was failed";
                        Logger::addLogMSG(error, Logger::LogType::video);
                    }


                    string simulationDir =  "simulations/" + oSim->getAlgoFileName() + "_" + oSim->getHouseFileName() + "/";
                    string imagesExpression = simulationDir + "image%5d.jpg";
                    int result = Encoder::encode(imagesExpression, oSim->getAlgoFileName()  + "_" + oSim->getHouseFileName()  + ".mpg");

                    if(result == -1)
                    {
                        string error = "Error: In the simulation " + oSim->getAlgoFileName() + " ," + oSim->getHouseFileName() + ": video file creation failed";
                        Logger::addLogMSG(error, Logger::LogType::video);
                    }

                    oSim->deleteMontageFiles();
                }
            }


            int nScore;

            // calculate and print score
            for(size_t i = 0; i < vSimulations.size(); i++)
            {
            	unique_ptr<OneSimulation> &oSim = vSimulations[i];

                if (oSim->GetSimulationState() == OneSimulation::AlgoMadeIllegalMove)
                    nScore = 0;
                else
                {
                    if(m_bIsDefaultScore)
                    {
                        nScore = oSim->CalculateScore(nWinnerSteps, bIsWinner, nSimulationSteps);
                    }
                    else
                    {
                        map<string, int> mScoreParams;

                        //Add all paramters
                        mScoreParams.insert(pair<string,int>("actual_position_in_competition", oSim->GetActualPositionInCompetition()));
                        mScoreParams.insert(pair<string,int>("simulation_steps", nSimulationSteps));
                        mScoreParams.insert(pair<string,int>("winner_num_steps", nWinnerSteps));
                        mScoreParams.insert(pair<string,int>("this_num_steps", oSim->getSteps()));
                        mScoreParams.insert(pair<string,int>("sum_dirt_in_house", oSim->getHouse().GetInitialAmounthOfDirt()));
                        mScoreParams.insert(pair<string,int>("dirt_collected", oSim->getHouse().GetDirtCollected()));

                        int nIsinDockling = (oSim->getHouse().isVacuumInDocking()) ? 1 : 0;
                        mScoreParams.insert(pair<string,int>("is_back_in_docking", nIsinDockling));

                        nScore = calc_score(mScoreParams);
                        if(nScore == -1 && !m_bIsScoreError)
                        {
                            m_mScoreErrorLock.lock();
                            string strError = "Score formula could not calculate some scores, see -1 in the results table";
                            Logger::addLogMSG(strError, Logger::LogType::Scores);
                            m_bIsScoreError = true;
                            m_mScoreErrorLock.unlock();

                        }
                    }
                }

                m_mScoreLock.lock();
                oScores[oSim->getAlgoFileName()][pHouse->GetHouseFileName()] = nScore;
                m_mScoreLock.unlock();

            if(bDebug)
            {
                cout << oSim->getHouse() << endl;

                cout << "This simulation state: ";
                switch(oSim->GetSimulationState())
                {
                case OneSimulation::SimulationStateType::AlgoMadeIllegalMove:
                    cout << "AlgoMadeIllegalMove"<< endl;
                    break;
                case OneSimulation::SimulationStateType::Finished:
                    cout << "Finished"<< endl;
                    break;
                case OneSimulation::SimulationStateType::OutOfBattery:
                    cout << "OutOfBattery"<< endl;
                    break;
                case OneSimulation::SimulationStateType::Running:
                    cout << "Running"<< endl;
                    break;
                }

                cout << "This simulation steps: " << oSim->getSteps() << endl;

                cout << endl;
            }
        }

        vSimulations.clear();
        Algos.clear();
        }
    }
}

// Runs the simulation
void Simulator::Run()
{
    for(unique_ptr<House> &pHouse : m_vOriginalHouses)
    {
        m_HouseQueue.push(std::move(pHouse));
    }

    if(m_nNumOfThreads > 1)
    {
        vector<thread> threads;
        for(int i = 0; i < m_nNumOfThreads; i++)
        {
            threads.push_back(thread(&Simulator::RunOnHouseThread,this));
        }

        for(auto& thread_ptr : threads) {
            thread_ptr.join();
        }
    }
    else
    {
        RunOnHouseThread();
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
    for(const auto &oHousesScoresPair : oScores[m_vAlgoFileNames[0]])
    {
        cout << left << setw(scoreWidth) << setfill(space) << (oHousesScoresPair.first).substr(0, min((int)oHousesScoresPair.first.length(), 9)) << seperator;
    }

    cout << left << setw(scoreWidth) << setfill(space) << "AVG" << seperator << endl;

    cout << setw(nLineN) << setfill(lineSep)<<  lineSep << endl;
    // for each algorithm: print scores
    vector<pair<string, map<string, int>>> vScores;
    for(const auto &oAlgoHousesPair: oScores)
    {
    	vScores.push_back(oAlgoHousesPair);
    }

    for(size_t i = 0; i < vScores.size(); i++)
    {
        int nCount = 0;
        int nSum = 0;

		for(const auto &oHousesScoresPair : vScores[i].second)
		{
			nCount++;
			nSum += oHousesScoresPair.second;
		}

		double dAvgi = nSum / (double)nCount;

    	for(size_t j = i + 1; j < vScores.size(); j++)
    	{
            int nCount = 0;
            int nSum = 0;

    		for(const auto &oHousesScoresPair : vScores[j].second)
    		{
    			nCount++;
    			nSum += oHousesScoresPair.second;
    		}

    		double dAvgj = nSum / (double)nCount;

    		if(dAvgj > dAvgi)
    		{
    			std::swap(vScores[i], vScores[j]);
    			dAvgi = dAvgj;
    		}
    	}
    }

    for(const auto &oAlgoHousesPair: vScores)
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
    /*for(OneSimulation *pSim : m_vSimulations)
          pSim;*/

    /*for(AbstractAlgorithm *pAlgo : m_vAlgos)
        delete pAlgo;*/

    /*for(House *pHouse : m_vOriginalHouses)
        delete pHouse;*/

	for(void* pHandle : m_vAlgoLibHandles)
		dlclose(pHandle);

    if(!m_bIsDefaultScore)
    {
        dlclose(m_ptrScoreHandle);
    }
}

// Make single simulation step
void Simulator::OneSimulation::MakeStep()
{
	Direction oDir = m_pAlgo->step(m_oPrevStep);

	const int i = m_oHouse.GetVacuumPos().i;
	const int j = m_oHouse.GetVacuumPos().j;

	if(STEP_MISTATCH)
	{
		// once every ten steps make a step in a random direction
		if(m_nSteps % 10 == 0)
		{
			vector<Direction> vPosibleDirections;

			if(m_oHouse[i - 1][j    ] != WALL_CELL) vPosibleDirections.push_back(Direction::North);
			if(m_oHouse[i    ][j + 1] != WALL_CELL) vPosibleDirections.push_back(Direction::East);
			if(m_oHouse[i + 1][j    ] != WALL_CELL) vPosibleDirections.push_back(Direction::South);
			if(m_oHouse[i    ][j - 1] != WALL_CELL) vPosibleDirections.push_back(Direction::West);

			vPosibleDirections.push_back(Direction::Stay);

			oDir = vPosibleDirections[rand() % vPosibleDirections.size()];
		}
	}

	m_oPrevStep = oDir;

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

string trim(string& str)
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
    string sScorePath = "";
    int    nThreads = 1;
    bool bCreateVideo = false;

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
        else if (SCORE_PATH_FLAG.compare(argv[i]) == 0)
        {
            if (i < (argsc - 1))
                sScorePath = argv[++i];
        }
        else if (THREAD_NUM_FLAG.compare(argv[i]) == 0)
        {
            if (i < (argsc - 1))
                nThreads = atoi(argv[++i]);
        }
        else if (VIDEO_FLAG.compare(argv[i]) == 0)
        {
            bCreateVideo = true;
        }
        else
        {
            cout << USAGE << endl;
            return 1;
        }
    }

    if((bCreateVideo) && (nThreads > 1))
    {
     cout << "Cannot Create Video While running with more then 1 thread" << endl;
     return 0;
    }

    sConfigPath = sConfigPath.length() == 0 ? "." : sConfigPath;

    // Add config Path dir sign id needed
    if (sConfigPath[sConfigPath.length() - 1] != PATH_SEPARATOR)
        sConfigPath += PATH_SEPARATOR;

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


    // Add score Path dir sign id needed
    if ((sScorePath.length() != 0) && (sScorePath[sScorePath.length() - 1] != PATH_SEPARATOR))
        sScorePath += PATH_SEPARATOR;

    try
    {
        Simulator sim(sConfigPath, sHousesPath, sAlgosPath, sScorePath, nThreads, bCreateVideo);
        sim.Run();

        // if we reached this point that means Simulator didn't throw an exception

        if(Logger::getLog(Logger::LogType::Houses).size() != 0 || Logger::getLog(Logger::LogType::Algos).size())
            cout << endl <<"Errors:" << endl;

        for(const string &sLogEntry : Logger::getLog(Logger::LogType::Houses, true))
            cout << sLogEntry << endl;

        for(const string &sLogEntry : Logger::getLog(Logger::LogType::Algos, true))
            cout << sLogEntry << endl;

        for(const string &sLogEntry : Logger::getLog(Logger::LogType::Scores, true))
            cout << sLogEntry << endl;

        for(const string &sLogEntry : Logger::getLog(Logger::LogType::video, true))
            cout << sLogEntry << endl;
    }
    catch (exception& ex)
    {
        cout << ex.what() << endl;
    }

    return 0;
}
