/*
 * Sensor.cpp
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#include "Sensor.h"

SensorInformation Sensor::sense() const
{
	SensorInformation si;
	int i = m_oHouse.GetVacuumPos().i;
	int j = m_oHouse.GetVacuumPos().j;

	if(m_oHouse[i][j] != 'D' && m_oHouse[i][j] != ' ')
		si.dirtLevel = m_oHouse[i][j] - '0';
	else
		si.dirtLevel = 0;

	si.isWall[0] = m_oHouse[i - 1][j    ] == WALL_CELL;
	si.isWall[1] = m_oHouse[i    ][j + 1] == WALL_CELL;
	si.isWall[2] = m_oHouse[i + 1][j    ] == WALL_CELL;
	si.isWall[3] = m_oHouse[i    ][j - 1] == WALL_CELL;

	return si;
}
