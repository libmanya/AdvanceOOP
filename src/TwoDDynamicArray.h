/*
 * TwoDDynamicArray.h
 *
 *  Created on: Apr 16, 2016
 *      Author: ilya
 */

#ifndef _TWOD_DYNAMIC_ARRAY_H_
#define _TWOD_DYNAMIC_ARRAY_H_

#define TDDA TwoDDynamicArray

class TwoDDynamicArray
{
	class MapRow
	{
		TDDA& m_oMap;
		int m_nRow;

	public:

		MapRow(TDDA& oMap, int nRow) : m_oMap(oMap), m_nRow(nRow){}

		char& operator[](size_t nCol)
		{
			return m_oMap.at(m_nRow, nCol);
		}

		const char& operator[] (size_t nCol) const
		{
			return m_oMap.at(m_nRow, nCol);
		}

	};

public:
	virtual char& at(int i, int j) = 0;
	virtual bool exists(int i, int j) = 0;

	MapRow operator[](int nRow)
	{
		return MapRow(*this, nRow);
	}

	virtual ~TwoDDynamicArray(){};
};



#endif /* _TWOD_DYNAMIC_ARRAY_H_ */
