/*
 * Simulator.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin & Yaronlibman
 */

//Hard Coded House
char house1[8][10] = { {'w','w', 'w', 'w', 'w', 'w', 'w', 'w', 'w','w'},
					   { 'w','2', '2', ' ', ' ', 'D', 'w', '5', '9','w' },
					   { 'w',' ', ' ', 'w', ' ', '1', '1', '1', '9','w' },
					   { 'w',' ', 'w', 'w', 'w', '3', 'w', 'w', ' ','w' },
					   { 'w','6', ' ', ' ', ' ', '3', 'w', ' ', ' ','w' },
					   { 'w','7', '8', 'w', ' ', ' ', 'w', ' ', ' ','w' },
					   { 'w','9', '9', 'w', ' ', ' ', 'w', ' ', ' ','w' },
					   { 'w','w', 'w', 'w', 'w', 'w', 'w', 'w', 'w','w' } };

#include<fstream>
#include "House.h";
using namespace std;


class Simulator
{
	public:
		//Simulator Ctor with (config file Path)
		Simulator(string fConfigFilePath);
		
		~Simulator();

	private:
		int m_nMaxSteps;
		int m_nMaxStepsAfterWinner;
		int m_nBatteryCapacity;
		int m_nBatteryConsumptionRate;
		int m_nBatteryRachargeRate;
};

Simulator::Simulator(string fConfigFilePath)
{
	//Read Config File to members
}

Simulator::~Simulator()
{
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
