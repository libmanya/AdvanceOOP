/*
 * NaiveAlgo.h
 *
 *  Created on: 19 Mar 2016
 *      Author: Home
 */

#ifndef NAIVEALGO_H_
#define NAIVEALGO_H_

#include "AbstractAlgorithm.h"
#include <cstdlib>
#include <ctime>

class NaiveAlgo: public AbstractAlgorithm
{
	const AbstractSensor *m_oSensor;
	map<string, int> m_oConfig;

public:

	NaiveAlgo(): m_oSensor(nullptr){std::srand(std::time(0));}

    void setSensor(const AbstractSensor& sensor) override
    {
    	m_oSensor = &sensor;
    }

    void setConfiguration(map<string, int> config) override
	{
    	m_oConfig = config;
	}

    Direction step() override;

    void aboutToFinish(int stepsTillFinishing) override;

    ~NaiveAlgo(){}
};

#endif /* NAIVEALGO_H_ */
