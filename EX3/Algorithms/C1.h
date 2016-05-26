/*
 * C1.h
 *
 *  Created on: May 20, 2016
 *      Author: ilya
 */

#ifndef C1_H_
#define C1_H_

#include "../Direction.h"
#include "../AbstractAlgorithm.h"
#include <cstdlib>
#include <ctime>
#include "TwoDDynamicArray.h"
#include "BFS.h"
#include "Utils.h"
#include "ExternalAlgo.h"
#include <cmath>
#include "../makeUnique.h"
#include "../AlgorithmRegistration.h"

class C1 : public AbstractAlgorithm
{

public:
	TDDA<char> m_oMatrix;  // TODO: change to private as well

private:

	enum AlgoState { FindWall, FollowWall, AdvanceToD, AtD, Finished };

	const AbstractSensor *m_pSensor;
	map<string, int> m_oConfig;

	Point m_oPos;
	unordered_set<char> m_oNonWallChars;
	AlgoState m_oCurrentState = FindWall;
	bool m_bAboutTofinish = false;
	int m_stepsTillFinishing;
	int m_nBatteryLevel;
	int m_nUnexploredOrDustyCellsCount = 0;
	TDDA<Direction> m_oPrevStepDirection;
	int m_nSteps;

	BFS::Path m_oCurrentPath;
	bool m_bIsCurrentPathInit = false;

	// For steps mismatch
	Direction m_oPrevStep;
	int m_nMismatchCount;

	//
	Point m_oWallBeingFollowed;
	bool m_bFirstPart = false;
	Direction m_oWallDirection;
	int m_nSteps2;

public:

	C1() : m_pSensor(nullptr), m_oPos({ 0, 0 })
	{
		m_oNonWallChars = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'D' , 'C', '~'};
		m_oMatrix[0][0] = 'D';

		m_oPrevStep = Direction::Stay;
		m_nMismatchCount = 0;

		m_nSteps = 0;
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

    Direction step(Direction prevStep) override;

	void aboutToFinish(int stepsTillFinishing) override;

	~C1() override {};

private:

	Direction HandleClean();
	Direction HandleFindWall();
	Direction HandleFollowWall();
	Direction HandleAdvanceToD();
	Direction HandleAtD();
	Direction HandleFinish();
	Direction HandleExploreDirtyAria();

	bool AboutToFinishOrLittleBattery(Point oPos, int nStepsOffset = 0);
	bool isAdjacentToWall(Point oPoint);
	bool isWallOrUnknown(Point oPoint);
};

REGISTER_ALGORITHM (C1)
#endif /* C1_H_ */
