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

	struct Point
	{
		int i, j;
	};	

	House(const string &sPath, int nBatteryCapacity, int nBatteryConsumptionRate, int nBatteryRechargeRate);
	House(const House &oFrom);
	House& operator=(const House &oFrom);
	friend ostream& operator<<(ostream& out, const House& oHouse);
	void TryMoveVacuum(Direction oDir);

	// Getters
	const Point& GetVacuumPos() 		const { return m_VacumPos; };
	int GetColNumber() 					const { return m_nColNumber; };
	int GetRowNumber() 					const { return m_nRowNumber; };
	const char* operator[](int i)		const { return m_pMap[i]; };   // for House[][] support (read only access to sells)
	int GetInitialAmounthOfDirt() 		const { return m_nInitialAmounthOfDirt; };
	int GetDirtCollected() 				const { return m_nDirtCollected; }
	int GetBatteryLevel() 				const { return m_BatteryLevel; };
	const string& GetHouseName()		const { return m_sHouseName; };
	const string& GetHouseDesc()		const { return m_sHouseDesc; };

	~House();

private:

	string m_sHouseName;
	string m_sHouseDesc;
	Point m_VacumPos;
	int m_nRowNumber;
	int m_nColNumber;
	char **m_pMap;

	// Dirt
	int m_nInitialAmounthOfDirt;
	int m_nDirtCollected = 0;

	// Battery
	int m_BatteryLevel;
	int m_BatteryCapacity;
	int m_BatteryConsumptionRate;
	int m_BatteryRechargeRate;

	void TryCollectDirt();
};

#endif /* HOUSE_H_ */
