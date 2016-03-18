#include<fstream>
#include "House.h";
using namespace std;


class Simulator
{
public:
	//Simulator Ctor with (config file Path)
	Simulator(string fConfigFilePath);
	~Simulator();

	int loadHouse(char**);

private:
	int m_nMaxSteps;
	int m_nMaxStepsAfterWinner;
	int m_nBatteryCapacity;
	int m_nBatteryConsumptionRate;
	int m_nBatteryRachargeRate;
};
