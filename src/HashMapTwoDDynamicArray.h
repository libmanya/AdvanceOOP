/*
 * HashMapTwoDDynamicArray.h
 *
 *  Created on: Apr 16, 2016
 *      Author: ilya
 */

#ifndef HASHMAPTWODDYNAMICARRAY_H_
#define HASHMAPTWODDYNAMICARRAY_H_

#define HMTDDA HashMapTwoDDynamicArray

#include "TwoDDynamicArray.h"
#include "House.h"
#include <unordered_map>

class HashMapTwoDDynamicArray : public TDDA
{
	std::unordered_map<Point, char> m_oMap;

public:
	HashMapTwoDDynamicArray(){};

	virtual char& at(int i, int j)
	{
		return m_oMap[{i, j}];
	}
	virtual bool exists(int i, int j)
	{
		return m_oMap.find({i, j}) != m_oMap.end();
	}
};


#endif /* HASHMAPTWODDYNAMICARRAY_H_ */
