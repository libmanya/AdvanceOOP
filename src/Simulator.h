#include <fstream>
#include <vector>
#include "House.h"
#include "AbstractSensor.h"
#include "AbstractAlgorithm.h"
#include "Sensor.h"
#include <map>

using namespace std;

class Simulator
{

public:
	//Simulator Ctor with (config file Path)
	Simulator(const string &sConfigFilePath, const string &HousesPath);
	~Simulator();
	void Run();

	class OneSimulation
	{

	public:
		enum SimulationStateType{ FinishedCleaning, OutOfBattery, AlgoMadeIllegalMove, Running};

		int m_nActualPositionInCompetition = 0;
		SimulationStateType SimulationState;

		OneSimulation(const House &oHouse, AbstractAlgorithm* pAlgo,map<string, int> &oConfig): m_oHouse(oHouse), m_oSensor(m_oHouse), m_pAlgo(pAlgo), m_config(oConfig)
		{
			m_pAlgo->setConfiguration(m_config);
			m_pAlgo->setSensor(m_oSensor);

			SimulationState = Running;
		};
		void MakeStep();
		int getSteps() const { return m_nSteps; };
		const House& getHouse(){ return m_oHouse; };
		void AnnounceAboutToFinish(){ m_pAlgo->aboutToFinish(m_config["MaxStepsAfterWinner"]); };

	private:

		House m_oHouse;
		Sensor m_oSensor;
		AbstractAlgorithm* m_pAlgo;
		map<string, int> &m_config;
		int m_nSteps = 0;
	};

private:

	void ReadConfig(const string &sConfigFilePath);
	void LoadHouses(const string &sHousesPath);
	void ReloadAlgorithms();
	void ReloadSensors();
	void ReloadSimulations(House *oHouse);

	vector<OneSimulation*> m_vSimulations;
	vector<House*> m_vOriginalHouses;
	vector<AbstractAlgorithm*> m_vAlgos;

	map<string, int> m_config;
};
