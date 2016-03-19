/*
 * Sensor.cpp
 *
 *  Created on: 19 Mar 2016
 *      Author: Home
 */

#include "Sensor.h"

SensorInformation Sensor::sense() const
{
	SensorInformation si;

	si.dirtLevel = m_oHouse.m_pMap[m_oHouse.m_VacumPos.y][m_oHouse.m_VacumPos.x];

	si.isWall[0] = (m_oHouse.m_pMap[m_oHouse.m_VacumPos.y][m_oHouse.m_VacumPos.x + 1] == 'w');
	si.isWall[1] = (m_oHouse.m_pMap[m_oHouse.m_VacumPos.y + 1][m_oHouse.m_VacumPos.x] == 'w');
	si.isWall[2] = (m_oHouse.m_pMap[m_oHouse.m_VacumPos.y][m_oHouse.m_VacumPos.x - 1] == 'w');
	si.isWall[3] = (m_oHouse.m_pMap[m_oHouse.m_VacumPos.y - 1][m_oHouse.m_VacumPos.x] == 'w');

	return si;
}
