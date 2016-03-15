#ifndef __ABSTRACT_SENSOR__H_
#define __ABSTRACT_SENSOR__H_

#include "SensorInformation.h"

class AbstractSensor 
{ 
public: 
    // returns the sensor's information of the current location of the robot 
    virtual SensorInformation sense() const = 0; 
	virtual void ~AbstractSensor() = 0;
};

#endif //__ABSTRACT_SENSOR__H_
