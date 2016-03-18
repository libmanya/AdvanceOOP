/*
 * Simulator.cpp
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin & Yaronlibman
 */

#include "Simulator.h"
 //Hard Coded House
char house1[8][10] = { { 'w','w', 'w', 'w', 'w', 'w', 'w', 'w', 'w','w' },
{ 'w','2', '2', ' ', ' ', 'D', 'w', '5', '9','w' },
{ 'w',' ', ' ', 'w', ' ', '1', '1', '1', '9','w' },
{ 'w',' ', 'w', 'w', 'w', '3', 'w', 'w', ' ','w' },
{ 'w','6', ' ', ' ', ' ', '3', 'w', ' ', ' ','w' },
{ 'w','7', '8', 'w', ' ', ' ', 'w', ' ', ' ','w' },
{ 'w','9', '9', 'w', ' ', ' ', 'w', ' ', ' ','w' },
{ 'w','w', 'w', 'w', 'w', 'w', 'w', 'w', 'w','w' } };
Simulator::Simulator(string fConfigFilePath)
{
	//Read Config File to members
}

Simulator::~Simulator()
{
}

int Simulator::loadHouse(char** map)
{
	House *newHouse = new House("House name Test",
		"This is my test house description",
		8, 10, (char**)house1);

	newHouse->PrintHouse();
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
