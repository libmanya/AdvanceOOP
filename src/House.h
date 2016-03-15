/*
 * House.h
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin
 */

#ifndef HOUSE_H_
#define HOUSE_H_

// TODO: Define map chars

#include <string>

using namespace std;

class House
{
	string m_sHouseName;
	string m_sHouseDesc;
	int m_nRowNumber;
	int m_nColNumber;

	char **m_pMap;

}

#endif /* HOUSE_H_ */
