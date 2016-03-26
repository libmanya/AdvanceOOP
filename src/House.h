/*
 * House.h
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin
 */
#ifndef HOUSE_H_
#define HOUSE_H_
#include <iostream>
#include <string>
#include "Direction.h"
using namespace std;

#define WALL_CELL 'W'
#define DOCKING_STATION_CELL 'D'
#define VACUUM_CELL 'R'
#define EMPTY_CELL ' '

class House
{
public:

	string m_sHouseName;
	string m_sHouseDesc;

	class Point
	{
	public:
		int i, j;
	};

	House(const string &sPath, int nBatteryCapacity, int nBatteryConsumptionRate, int nBatteryRechargeRate);
	House(const House &oFrom);
	House& operator=(const House &oFrom);
	void PrintHouse() const;
	void MoveVacuum(Direction oDir);

	// Getters
	const Point& GetVacuumPos() 		const { return m_VacumPos; };
	int GetColNumber() 					const { return m_nColNumber; };
	int GetRowNumber() 					const { return m_nRowNumber; };
	const char* operator[](const int i) const { return m_pMap[i]; };   // for House[][] support (read only access to sells)
	int GetInitialAmounthOfDirt() 		const { return m_nInitialAmounthOfDurt; };
	int GetDirtCollected() 				const { return m_nDurtCollected; }
	int GetBatteryLevel() 				const { return m_BatteryLevel; };

	~House();

private:
	Point m_VacumPos;
	int m_nRowNumber;
	int m_nColNumber;
	char **m_pMap;

	// Dirt
	int m_nInitialAmounthOfDurt;
	int m_nDurtCollected = 0;

	// Battery
	int m_BatteryLevel;
	int m_BatteryCapacity;
	int m_BatteryConsumptionRate;
	int m_BatteryRechargeRate;

	void TryCollectDirt();
};

#endif /* HOUSE_H_ */
