/*
 * Simulator.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin & Yaronlibman
 */

#include "Simulator.h"
#include "Sensor.h"
#include "NaiveAlgo.h"

Simulator::Simulator(const string &sConfigFilePath, const string &sHousesPath)
{
	//Read Config File to members
	ReadConfig(sConfigFilePath);

	// Load houses
	LoadHouses(sHousesPath);
	m_AlgoCount = 1;

	m_vAlgos.assign(m_AlgoCount, nullptr);
	m_vSensors.assign(m_AlgoCount, nullptr);
}

void Simulator::ReadConfig(const string &sConfigFilePath)
{
	// TODO: Read from actual config.ini file
	map<string, int> m_config;
	m_config["MaxSteps"] = 1200;
	m_config["MaxStepsAfterWinner"] = 200;
	m_config["BatteryCapacity"] = 400;
	m_config["BatteryConsumptionRate"] = 2;
	m_config["BatteryRachargeRate"] = 20;
}

void Simulator::LoadHouses(const string &sHousesPath)
{
	// TODO: add iteration over all *.house files in sHousesPath folder

	m_vOriginalHouses.push_back(new House(""));
}

void Simulator::ReloadAlgorithms()
{
	for(AbstractAlgorithm *algo:m_vAlgos)
		delete algo;

	m_vAlgos[0] = new NaiveAlgo();

	for(int i = 0; i < m_AlgoCount; i++)
	{
		m_vAlgos[i]->setSensor(*m_vSensors[i]);
		m_vAlgos[i]->setConfiguration(m_config);
	}
}

void Simulator::ReloadSensors()
{
	for(AbstractSensor *sensor:m_vSensors)
		delete sensor;

	for(int i = 0; i < m_AlgoCount; i++)
		m_vSensors[i] = new Sensor(m_vHouses[i]);

}

void Simulator::Run()
{
	for(House *h : m_vOriginalHouses)
	{
		m_vHouses.assign(m_AlgoCount, *h);
		ReloadSensors();
		ReloadAlgorithms();

	}

}

Simulator::~Simulator()
{
}

// MAIN
int main(int argsc, char **argv)
{
	// TODO: Process argument and pass them to simulator

	Simulator sim("", "");

	sim.Run();
	cout << "Here" << endl;
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
