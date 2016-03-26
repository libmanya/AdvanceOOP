/*
 * Simulator.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: Ilya Aizin & Yaron Libman
 */

#include "Simulator.h"
#include "NaiveAlgo.h"
#include <algorithm>
#include <sstream>

using namespace std;

Simulator::Simulator(const string &sConfigFilePath, const string &sHousesPath)
{
	//Read Configuration File to members
	ReadConfig(sConfigFilePath);

	// Load houses
	LoadHouses(sHousesPath);
}

// Reads configuration file and sets m_config keys
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
				throw std::runtime_error("ERROR IN NAVIGATION");
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

// Initializes houses (In exercise 1 there is only 1 hard-coded house)
void Simulator::LoadHouses(const string &sHousesPath)
{
	m_vOriginalHouses.push_back(new House("", m_config[BATTERY_CAPACITY_KEY], m_config[BATTERY_CONSUMPTION_KEY], m_config[BATTERY_RECHARGE_KEY]));
}

// Reloads simulations
void Simulator::ReloadSimulations(House *oHouse)
{
	for(OneSimulation *pSim : m_vSimulations)
		delete pSim;

	m_vSimulations.clear();

	for(AbstractAlgorithm *pAlgo : m_vAlgos)
		m_vSimulations.push_back(new Simulator::OneSimulation(*oHouse, pAlgo, m_config));
}

// Reloads algorithms
void Simulator::ReloadAlgorithms()
{
	for(AbstractAlgorithm *pAlgo : m_vAlgos)
		delete pAlgo;

	m_vAlgos.clear();

	m_vAlgos.push_back(new NaiveAlgo());
}

// Runs the simulation
void Simulator::Run()
{
	// For every house ran all simulations in paralel
	for(House *pHouse : m_vOriginalHouses)
	{
		ReloadAlgorithms();
		ReloadSimulations(pHouse);

		bool bSomeActive = true;
		bool bIsWinner = false;
		bool bAnnounceWinner = false;
		int nSimulationSteps = 0;
		int nWinnerSteps = 0;
		int lastFinnishedSteps = 0;
		int lastFinishedActualPositionInCopmetition = 0;

		// Run until some algorithms didn't finish and simulation maximum steps count was not reached
		while(bSomeActive
					&& nSimulationSteps < m_config[MAX_STEPS_KEY]
					&& (!bIsWinner || nSimulationSteps < nWinnerSteps + m_config[MAX_STEPS_AFTER_KEY]))
		{
			bSomeActive = false;
			for(OneSimulation *oSim : m_vSimulations)
			{

				if(oSim->GetSimulationState() == OneSimulation::Running)
				{
					bSomeActive = true;

					if(bAnnounceWinner || (nSimulationSteps == (m_config[MAX_STEPS_KEY] - m_config[MAX_STEPS_AFTER_KEY])))
					{
						bAnnounceWinner = false;
						oSim->AnnounceAboutToFinish();
					}

					oSim->MakeStep();

					if(oSim->GetSimulationState() == OneSimulation::FinishedCleaning)
					{
						// calculate Actual Position In Competition according to forum post by Amir
						oSim->SetActualPositionInCompetition(lastFinishedActualPositionInCopmetition + (lastFinnishedSteps == nSimulationSteps + 1 ? 0 : 1));
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

		// calculate score
		for(OneSimulation *oSim : m_vSimulations)
		{

			int nScore = oSim->CalculateScore(nWinnerSteps, bIsWinner, nSimulationSteps);
			cout << oSim->getHouse().GetHouseName() << '\t' << nScore << endl;
		}
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

int Simulator::OneSimulation::CalculateScore(int nWinnerSteps, bool bIsWinner, int nSimulationSteps) const
{
	int nScore;
	if (SimulationState == OneSimulation::AlgoMadeIllegalMove)
		nScore = 0;
	else
	{
		// calculate position in competion
		int nPositionInCompetition;
		if (SimulationState == OneSimulation::FinishedCleaning)
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
			if (i < (argsc - 1))
				strConfigPath = argv[++i];
		}
		else if (HOUSE_PATH_FLAG.compare(argv[i]) == 0)
		{
			if (i < (argsc - 1))
				strHousesPath = argv[++i];
		}
		else
		{
			cout << "Usage: simulator [-config <config_file_location >] [-house_path <houses_path_location>]" << endl;
			return 1;
		}
	}

	//Add config Path dir sign id needed
	if ((strConfigPath.length() > 0) && strConfigPath[strConfigPath.length() - 1] != PATH_SEPARATOR)
	{
		strConfigPath += PATH_SEPARATOR;
	}

	//Concat file name
	strConfigPath += CONFIG_FILE_NAME;
	
	try
	{
		Simulator sim(strConfigPath, "");
		sim.Run();
	}
	catch (const std::exception& e)
	{
		std::cout << "Caught exception: " << e.what() << '\n';
		cout << "here" << i << endl;
	}

	return 0;
}
