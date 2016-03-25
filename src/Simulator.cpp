/*
 * Simulator.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin & Yaronlibman
 */

#include "Simulator.h"

#include "NaiveAlgo.h"

Simulator::Simulator(const string &sConfigFilePath, const string &sHousesPath)
{
	//Read Configuration File to members
	ReadConfig(sConfigFilePath);

	// Load houses
	LoadHouses(sHousesPath);
}

void Simulator::ReadConfig(const string &sConfigFilePath)
{
	// TODO: Read from actual config.ini file
	m_config["MaxSteps"] = 1200;
	m_config["MaxStepsAfterWinner"] = 200;
	m_config["BatteryCapacity"] = 400;
	m_config["BatteryConsumptionRate"] = 1;
	m_config["BatteryRachargeRate"] = 20;
}

void Simulator::LoadHouses(const string &sHousesPath)
{
	m_vOriginalHouses.push_back(new House("", m_config["BatteryCapacity"], m_config["BatteryConsumptionRate"], m_config["BatteryRachargeRate"]));
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
					&& nSimulationSteps < m_config["MaxSteps"]
					&& (!bIsWinner || nSimulationSteps < nWinnerSteps + m_config["MaxStepsAfterWinner"]))
		{
			bSomeActive = false;
			for(OneSimulation *oSim : m_vSimulations)
			{
				if(oSim->SimulationState == OneSimulation::Running)
				{
					bSomeActive = true;

					if(bAnnounceWinner || (nSimulationSteps == m_config["MaxSteps"] - m_config["MaxStepsAfterWinner"]))
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

// MAIN
int main(int argsc, char **argv)
{
	// TODO: Process argument and pass them to simulator

	Simulator sim("", "");

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
