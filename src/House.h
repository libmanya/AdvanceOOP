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

	House(string name, string  Desc, int row, int col , char** Map);
	void PrintHouse();

private:
	string  m_sHouseName;
	string  m_sHouseDesc;
	int m_nRowNumber;
	int m_nColNumber;
	char **m_pMap;

};

#endif /* HOUSE_H_ */