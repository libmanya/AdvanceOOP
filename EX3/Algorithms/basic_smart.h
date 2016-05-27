/*
 * basic_smart.h
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef basic_smart_H_
#define basic_smart_H_

#include "../Direction.h"
#include "../AbstractAlgorithm.h"
#include <cstdlib>
#include <ctime>
#include "TwoDDynamicArray.h"
#include "BFS.h"
#include "Utils.h"
#include "../makeUnique.h"
#include "../AlgorithmRegistration.h"


class basic_smart : public AbstractAlgorithm
{

public:
	TDDA<char> m_oMatrix;  // TODO: change to private as well

private:

	enum AlgoState { Clean, AdvanceToClean, AdvanceToD, AtD, Finished };

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

	BFS::Path m_oPathToClean;
	bool m_bIsPathToCleanInit = false;

	BFS::Path m_oPathToD;
	bool m_bIsPathToDInit = false;

	// For steps mismatch
	Direction m_oPrevStep;

public:

	basic_smart() : m_pSensor(nullptr), m_oPos({ 0, 0 })
	{ 
		m_oNonWallChars = { ' ', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'D' };
		m_oMatrix[0][0] = 'D'; 

		m_oPrevStep = Direction::Stay;
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

	~basic_smart()
	{
		//cout << m_oMatrix << endl;

	}

private:

	Direction HandleClean();
	Direction HandleAdvanceToClean();
	Direction HandleAdvanceToD();
	Direction HandleAtD();
	Direction HandleFinish();
};

REGISTER_ALGORITHM (basic_smart)
#endif /* basic_smart_H_ */
