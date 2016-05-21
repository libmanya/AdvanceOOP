/*
 * A1.h
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef A1_H_
#define A1_H_

#include "../Direction.h"
#include "../AbstractAlgorithm.h"
#include <cstdlib>
#include <ctime>
#include "TwoDDynamicArray.h"
#include "BFS.h"
#include "Utils.h"
#include "ExternalAlgo.h"
#include <cmath>

class A1 : public AbstractAlgorithm
{

public:
	TDDA<char> m_oMatrix;  // TODO: change to private as well

private:

	enum AlgoState { Clean, AdvanceToClean, AdvanceToD, AtD, Finished, ExploreDirtyAria };

	const AbstractSensor *m_pSensor;
	map<string, int> m_oConfig;

	Point m_oPos;
	unordered_set<char> m_oNonWallChars;
	AlgoState m_oCurrentState = AdvanceToClean;
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

	A1() : m_pSensor(nullptr), m_oPos({ 0, 0 })
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

	~A1()
	{/*

		// run BFS to calculate distance to D
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { 'D' }, m_oNonWallChars);

		double nMismatchFrequency = ((m_nSteps != 0) ? (((double)m_nMismatchCount) / ((double)m_nSteps)) : 0.0);

		int stepsToGetBack = result.getDistance() + 1;

		stepsToGetBack += ceil(stepsToGetBack*nMismatchFrequency) * 2;  // take in account the fact that mismatches may occur

		cout << stepsToGetBack << endl;
*/
	}

private:

	Direction HandleClean();
	Direction HandleAdvanceToClean();
	Direction HandleAdvanceToD();
	Direction HandleAtD();
	Direction HandleFinish();
	Direction HandleExploreDirtyAria();

	bool AboutToFinishOrLittleBattery(Point oPos, int nStepsOffset = 0);
};

#endif /* A1_H_ */
