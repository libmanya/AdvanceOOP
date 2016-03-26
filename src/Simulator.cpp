/*
 * Simulator.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin & Yaronlibman
 */

#include "Simulator.h"
#include "NaiveAlgo.h"
#include <algorithm>
#include <sstream>

using namespace std;

static std::string trim(std::string& str);

string CONFIG_PATH_FLAG = "-config";
string HOUSE_PATH_FLAG = "-house";
string CONFIG_FILE_NAME = "config.ini";
string HOUSES_FILE_SUFFIX = ".house";

string BATTERY_CAPACITY_KEY = "BatteryCapacity";
string BATTERY_CONSUMPTION_KEY = "BatteryConsumptionRate";
string BATTERY_RACHARGE_KEY = "BatteryRachargeRate";
string MAX_STEPS_KEY = "MaxSteps";
string MAX_STEPS_AFTER_KEY = "MaxStepsAfterWinner";




Simulator::Simulator(const string &sConfigFilePath, const string &sHousesPath)
{
	//Read Configuration File to members
	ReadConfig(sConfigFilePath);

	// Load houses
	LoadHouses(sHousesPath);
}

void Simulator::ReadConfig(const string &sConfigFilePath)
{
	ifstream fin(sConfigFilePath);
	string line;

	//check if file open with path, if not try open file in current dir
	if (!fin)
	{
		if (sConfigFilePath.compare(CONFIG_FILE_NAME) != 0)
		{
			fin.open(CONFIG_FILE_NAME);
			if (!fin)
			{
				cout << "error Couldn't find Config file" << endl;
			}
		}
	}

	while (getline(fin, line))
	{
		stringstream ss(line);
		string item;
		vector<string> tokens;
		while (getline(ss, item, '=')) {
			tokens.push_back(item);
		}

		m_config[trim(tokens[0])] = stoi(trim(tokens[1]));
	}
		
}

void Simulator::LoadHouses(const string &sHousesPath)
{
	m_vOriginalHouses.push_back(new House("", m_config[BATTERY_CAPACITY_KEY], m_config[BATTERY_CONSUMPTION_KEY], m_config[BATTERY_RACHARGE_KEY]));
}

void Simulator::ReloadSimulations(House *oHouse)
{
	for(OneSimulation *pSim : m_vSimulations)
		delete pSim;

	m_vSimulations.clear();

	for(AbstractAlgorithm *pAlgo : m_vAlgos)
		m_vSimulations.push_back(new Simulator::OneSimulation(*oHouse, pAlgo, m_config));
}

void Simulator::ReloadAlgorithms()
{
	for(AbstractAlgorithm *pAlgo : m_vAlgos)
		delete pAlgo;

	m_vAlgos.clear();

	m_vAlgos.push_back(new NaiveAlgo());
}

void Simulator::Run()
{
	for(House *pHouse : m_vOriginalHouses)
	{
		ReloadAlgorithms();
		ReloadSimulations(pHouse);

		bool bSomeActive = true;
		bool bIsWinner = false;
		bool bAnnounceWinner = false;
		int nSimulationSteps = 0;
		int nWinnerSteps;

		int lastFinnishedSteps = 0;
		int lastFinishedActualPositionInCopmetition = 0;

		//pHouse->PrintHouse();
		while(bSomeActive
					&& nSimulationSteps < m_config[MAX_STEPS_KEY]
					&& (!bIsWinner || nSimulationSteps < nWinnerSteps + m_config[MAX_STEPS_AFTER_KEY]))
		{
			bSomeActive = false;
			for(OneSimulation *oSim : m_vSimulations)
			{
				if(oSim->SimulationState == OneSimulation::Running)
				{
					bSomeActive = true;

					if(bAnnounceWinner || (nSimulationSteps == (m_config[MAX_STEPS_KEY] - m_config[MAX_STEPS_AFTER_KEY])))
					{
						bAnnounceWinner = false;
						oSim->AnnounceAboutToFinish();
					}

					oSim->MakeStep();

					if(oSim->SimulationState == OneSimulation::FinishedCleaning)
					{
						// calculate Actual Position In Competition according to forum post by Amir
						oSim->m_nActualPositionInCompetition = lastFinishedActualPositionInCopmetition + (lastFinnishedSteps == nSimulationSteps + 1 ? 0 : 1);
						lastFinnishedSteps = nSimulationSteps + 1;  // +1 because winner was declared after the step

						if(!bIsWinner)
						{
							bIsWinner = true;
							nWinnerSteps = lastFinnishedSteps;
							bAnnounceWinner = true; 		// remember to announce winner at the beginning of the next round
						}

						cout << "One Finished" << endl;
					}

				}

			}

			nSimulationSteps++;
		}

		for(OneSimulation *oSim : m_vSimulations)
		{
			// calculate score
			int nPositionInCopmetition;

			if(oSim->SimulationState == OneSimulation::FinishedCleaning)
				nPositionInCopmetition = std::min(4, oSim->m_nActualPositionInCompetition);
			else
				nPositionInCopmetition = 10;

			if(!bIsWinner)
				nWinnerSteps = nSimulationSteps;

			int nScore;
			nScore = std::max(0,
					2000
					- (nPositionInCopmetition - 1) * 50
					+ (nWinnerSteps - oSim->getSteps()) * 10
					- (oSim->getHouse().GetInitialAmounthOfDirt() - oSim->getHouse().GetDirtCollected()) * 3
					+ (oSim->getHouse()[oSim->getHouse().GetVacuumPos().i][oSim->getHouse().GetVacuumPos().j] == DOCKING_STATION_CELL ? 50: -200));

			cout << oSim->getHouse().m_sHouseName << '\t' << nScore << endl;
		}

		//m_vSimulations[0]->getHouse().PrintHouse();
		//cout << m_vSimulations[0]->getSteps() << endl;

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

void Simulator::OneSimulation::MakeStep()
{
	if(m_oHouse.GetBatteryLevel() == 0)
	{
			SimulationState = OutOfBattery;
			return;
	}

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
	m_oHouse.MoveVacuum(oDir);

	m_nSteps++;

	if(m_oHouse.GetDirtCollected() == m_oHouse.GetInitialAmounthOfDirt())
		SimulationState = FinishedCleaning;
}

static std::string trim(std::string& str)
{
	str.erase(0, str.find_first_not_of(' '));     
	str.erase(str.find_last_not_of(' ') + 1);         
	return str;
}



// MAIN
int main(int argsc, char **argv)
{
	int i;
	string strConfigPath = "";
	string strHousesPath = "";

	// Gets Command line parameters
	for (i = 1; i < argsc; i++)
	{
		if (CONFIG_PATH_FLAG.compare(argv[i]) == 0)
		{
			if (i < (argsc - 1)) {
				strConfigPath = argv[++i];
			}
		}
		else if (HOUSE_PATH_FLAG.compare(argv[i]) == 0)
		{
			if (i < (argsc - 1)) {
				strHousesPath= argv[++i];
			}
		}
	}

	//Add config Path dir sign id needed
	if (strConfigPath.at(strConfigPath.length() - 1) != '\\')
	{
		strConfigPath += "\\";
	}

	//Concat file name
	strConfigPath += CONFIG_FILE_NAME;
	
	Simulator sim(strConfigPath, "");

	sim.Run();

	return 0;
}



// simulatorInit


// Run
	// read config.init
	// load and create houses
	// init sensors
	// init algorithms
	// run all algorithms on all houses
		// run single step on all the algos
		// check wining or illegal steps conditions
			//Declare winner

// RunSingleStep (on a single house)

// getScore
