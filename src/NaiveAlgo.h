/*
 * NaiveAlgo.h
 *
 *  Created on: 19 Mar 2016
 *      Author: Home
 */

#ifndef NAIVEALGO_H_
#define NAIVEALGO_H_

#include "AbstractAlgorithm.h"

class NaiveAlgo: public AbstractAlgorithm
{
public:
    void setSensor(const AbstractSensor& sensor){}

    void setConfiguration(map<string, int> config){}

    Direction step(){return Direction();}

    void aboutToFinish(int stepsTillFinishing){}

    ~NaiveAlgo(){}
};

#endif /* NAIVEALGO_H_ */
