#include <fstream>
#include <vector>
#include "House.h"
#include "AbstractSensor.h"
#include "AbstractAlgorithm.h"
#include <map>

using namespace std;

class Simulator
{
public:
	//Simulator Ctor with (config file Path)
	Simulator(const string &sConfigFilePath, const string &HousesPath);
	~Simulator();
	void Run();

private:

	void ReadConfig(const string &sConfigFilePath);
	void LoadHouses(const string &sHousesPath);
	void ReloadAlgorithms();
	void ReloadSensors();

	vector<House*> m_vOriginalHouses;
	vector<AbstractSensor*> m_vSensors;
	vector<AbstractAlgorithm*> m_vAlgos;
	vector<House> m_vHouses;

	map<string, int> m_config;
	int m_AlgoCount;
};
