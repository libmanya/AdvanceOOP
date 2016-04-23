/*
 * NaiveAlgo.h
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef _323500942_B_H_
#define _323500942_B_H_

#include "Direction.h"
#include "AbstractAlgorithm.h"
#include <cstdlib>
#include <ctime>
#include "TwoDDynamicArray.h"
#include "House.h"
#include "BFS.h"
#include "Utils.h"

class _323500942_B : public AbstractAlgorithm
{

public:
	TDDA<char> m_oMatrix;  // TODO: change to private as well

private:

	enum AlgoState { Clean, Explore, AdvanceToD, AtD, Finished };

	const AbstractSensor *m_pSensor;
	map<string, int> m_oConfig;

	Point m_oPos;
	unordered_set<char> m_oNonWallChars;
	AlgoState m_oCurrentState = Explore;
	bool m_bAboutTofinish = false;
	int m_stepsTillFinishing;
	int m_nBatteryLevel;
	int m_nUnexploredOrDustyCellsCount = 0;
	TDDA<Direction> m_oPrevStepDirection;

public:

	_323500942_B() : m_pSensor(nullptr), m_oPos({ 0, 0 })
	{ 
		m_oNonWallChars = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'D' };
		m_oMatrix[0][0] = 'D'; 
	}

    void setSensor(const AbstractSensor& sensor) override
    {
    	m_pSensor = &sensor;
    }

    void setConfiguration(map<string, int> config) override
	{
    	m_oConfig = config;
		m_nBatteryLevel = config[BATTERY_CAPACITY_KEY];
	}

    Direction step() override;

	void aboutToFinish(int stepsTillFinishing) override;

	~_323500942_B() {}

private:

	Direction HandleClean();
	Direction HandleExplore();
	Direction HandleAdvanceToD();
	Direction HandleAtD();
	Direction HandleFinish();
};

#endif /* _323500942_B_H_ */
