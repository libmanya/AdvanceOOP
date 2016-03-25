/*
 * Sensor.h
 *
 *  Created on: 19 Mar 2016
 *      Author: Home
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include "House.h"
#include "AbstractSensor.h"

class Sensor : public AbstractSensor
{
public:
	House& m_oHouse;

	Sensor(House &oHouse):m_oHouse(oHouse){};
	SensorInformation sense() const override;
	~Sensor(){};
};



#endif /* SENSOR_H_ */
