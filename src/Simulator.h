/*
* Simulator.cpp
*
*  Created on: Mar 15, 2016
*      Author: iliyaaizin 323500942 & yaronlibman 302730072
*/

#include <fstream>
#include <vector>
#include "Direction.h"
#include "House.h"
#include "AbstractSensor.h"
#include "AbstractAlgorithm.h"
#include "Sensor.h"
#include <map>

#if defined(WIN32) || defined(_WIN32)
#define PATH_SEPARATOR '\\'
#else
#define PATH_SEPARATOR '/'
#endif

static std::string trim(std::string& str);

const string CONFIG_PATH_FLAG = "-config";
const string HOUSE_PATH_FLAG = "-house_path";
const string ALGO_PATH_FLAG = "-algorithm_path";
const string CONFIG_FILE_NAME = "config.ini";
const string HOUSES_FILE_SUFFIX = ".house";
const string ALGO_FILE_SUFFIX = "_.so";

const string BATTERY_CAPACITY_KEY = "BatteryCapacity";
const string BATTERY_CONSUMPTION_KEY = "BatteryConsumptionRate";
const string BATTERY_RECHARGE_KEY = "BatteryRechargeRate";
const string MAX_STEPS_AFTER_KEY = "MaxStepsAfterWinner";

const string MAX_STEPS_KEY = "MaxSteps";

using namespace std;

class Simulator
{

public:
	//Simulator Ctor with (config file Path)
	Simulator(const string &sConfigFilePath, const string &sHousesPath , const string &sAlgosPath);
	~Simulator();
	void Run();

	class OneSimulation
	{

	public:
		enum SimulationStateType{ Finished, OutOfBattery, AlgoMadeIllegalMove, Running };

		OneSimulation(const House &oHouse, AbstractAlgorithm* pAlgo, map<string, int> &oConfig): m_oHouse(oHouse), m_oSensor(m_oHouse), m_pAlgo(pAlgo), m_config(oConfig)
		{
			m_pAlgo->setConfiguration(m_config);
			m_pAlgo->setSensor(m_oSensor);

			SimulationState = Running;
		};
		void MakeStep();
		int getSteps()							const	{ return m_nSteps; };
		const House& getHouse()					const	{ return m_oHouse; };
		void AnnounceAboutToFinish()					{ m_pAlgo->aboutToFinish(m_config["MaxStepsAfterWinner"]); };
		int GetActualPositionInCompetition()	const	{ return m_nActualPositionInCompetition; }
		SimulationStateType GetSimulationState()const	{ return SimulationState; }
		int CalculateScore(int nWinnerSteps,	bool bIsWinner, int nSimulationSteps)
												const;
		void SetActualPositionInCompetition(int nPos)	{ m_nActualPositionInCompetition = nPos;  }

	private:

		SimulationStateType SimulationState;
		House m_oHouse;
		Sensor m_oSensor;
		AbstractAlgorithm* m_pAlgo;
		map<string, int> &m_config;
		int m_nSteps = 0;
		int m_nActualPositionInCompetition = 0;
	};

private:

	void ReadConfig(const string &sConfigFilePath);
	void LoadHouses(const string &sHousesPath);
	void LoadAlgos(std::vector<string> &vDirAlgosFiles, const string &sHousesPath);
	void ReloadAlgorithms();
	void ReloadSimulations(House *oHouse);

	vector<OneSimulation*> m_vSimulations;
	vector<House*> m_vOriginalHouses;
	vector<AbstractAlgorithm*> m_vAlgos;

	map<string, int> m_config;
};
