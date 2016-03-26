/*
 * NaiveAlgo.cpp
 *
 *  Created on: 19 Mar 2016
 *      Author: Ilya Aizin & Yaron Libman
 */

#include <vector>
#include "NaiveAlgo.h"

Direction NaiveAlgo::step()
{
	SensorInformation oSI = m_oSensor->sense();
	vector<Direction> vPossibleDirs;

	// add all direction that don't lead to walls to Possible Directions
	if(oSI.isWall[0] == false) vPossibleDirs.push_back(Direction::North);
	if(oSI.isWall[1] == false) vPossibleDirs.push_back(Direction::East);
	if(oSI.isWall[2] == false) vPossibleDirs.push_back(Direction::South);
	if(oSI.isWall[3] == false) vPossibleDirs.push_back(Direction::West);
	vPossibleDirs.push_back(Direction::Stay);

	// return a random direction
	return vPossibleDirs[rand() % vPossibleDirs.size()];
}

void NaiveAlgo::aboutToFinish(int stepsTillFinishing)
{
}
