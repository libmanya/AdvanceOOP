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
using namespace std;

// TODO: Define map chars

class House
{
public:

	class Point
	{
	public:
		int x, y;
	};

	House(const string &sPath);
	House(const House &oFrom);
	House& operator=(const House &oFrom);
	void PrintHouse() const;
	~House();

	Point m_VacumPos;
	string m_sHouseName;
	string m_sHouseDesc;
	int m_nRowNumber;
	int m_nColNumber;
	char **m_pMap;
};

#endif /* HOUSE_H_ */
