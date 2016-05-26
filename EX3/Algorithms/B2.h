/*
 * B2.h
 *
 *  Created on: May 19, 2016
 *      Author: ilya
 */

#ifndef B2_H_
#define B2_H_

/*
 * B2.h
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

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
class B2 : public AbstractAlgorithm
{

public:
	TDDA<char> m_oMatrix;  // TODO: change to private as well

private:

	enum AlgoState { Clean, FindWall, FollowWall, AdvanceToD, AtD, Finished, ExploreDirtyAria };

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

	Point m_oLastDiscoveredDirtyAriaBorder;
	unordered_map<Point, int> oVisitNumber;
	vector<Point> m_vFoundDirtyCells;

public:

	B2() : m_pSensor(nullptr), m_oPos({ 0, 0 })
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

	~B2() override {};

private:

	Direction HandleClean();
	Direction HandleFindWall();
	Direction HandleFollowWall();
	Direction HandleAdvanceToD();
	Direction HandleAtD();
	Direction HandleFinish();
	Direction HandleExploreDirtyAria();

	bool AboutToFinishOrLittleBattery(Point oPos, int nStepsOffset = 0);
	bool oneLineDirtyAria(const vector<Point> &vFoundDirtyCells);
	bool isAdjacentToWall(Point oPoint);
};

REGISTER_ALGORITHM (B2)

#endif /* B2_H_ */
