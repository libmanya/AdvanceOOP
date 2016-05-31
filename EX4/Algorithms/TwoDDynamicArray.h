/*
 * TwoDDynamicArray.h
 *
 *  Created on: Apr 16, 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef _TWOD_DYNAMIC_ARRAY_H_
#define _TWOD_DYNAMIC_ARRAY_H_

#include <unordered_map>
#include "Utils.h"

template <class T> class TwoDDynamicArray;

template<class T>
using TDDA = TwoDDynamicArray<T>;

template <class T> class TwoDDynamicArray
{
	std::unordered_map<Point, T> m_oMap;

	template <class G> class MapRow
	{
		TDDA<G>& m_oMap;
		int m_nRow;

	public:
		MapRow(TDDA<G>& oMap, int nRow) : m_oMap(oMap), m_nRow(nRow){}
		T& operator[](size_t nCol) { return m_oMap.at(m_nRow, nCol); }
	};

	template <class G> class ConstMapRow
	{
		const TDDA<G>& m_oMap;
		int m_nRow;

	public:
		ConstMapRow(const TDDA<G>& oMap, int nRow) : m_oMap(oMap), m_nRow(nRow){}
		const T& operator[] (size_t nCol) const { return m_oMap.at(m_nRow, nCol); }
	};

public:
	T& at(int i, int j) 
	{
		return m_oMap[{i, j}];
	}

	const T& at(int i, int j) const
	{
		return m_oMap.at({ i, j });
	}

	T& at(Point point)
	{
		return at(point.i, point.j);
	}

	const T& at(Point point) const
	{
		return at(point.i, point.j);
	}

	bool exists(int i, int j) const
	{
		return m_oMap.find({ i, j }) != m_oMap.end();
	}

	bool exists(Point point) const
	{
		return exists(point.i, point.j);
	}

	MapRow<T> operator[](int nRow)
	{
		return MapRow<T>(*this, nRow);
	}

	ConstMapRow<T> operator[](int nRow) const
	{
		return ConstMapRow<T>(*this, nRow);
	}

	T& operator[](const Point &p)
	{
		return (*this)[p.i][p.j];
	}

	const T& operator[](const Point &p) const
	{
		return (*this)[p.i][p.j];
	}

	const std::unordered_map<Point, T>& getMap() const
	{
		return m_oMap;
	}

	//template <class G>
	friend ostream &operator<<(ostream &out, const TDDA<T> &oMatrix)
	{
		int nFarNorth = 1, nFarEast = -1, nFarSouth = -1, nFarWest = 1;

		for (auto iter = oMatrix.getMap().cbegin(); iter != oMatrix.getMap().cend(); iter++)
		{
			if (iter->first.i > nFarSouth)
				nFarSouth = iter->first.i;

			if (iter->first.i < nFarNorth)
				nFarNorth = iter->first.i;

			if (iter->first.j > nFarEast)
				nFarEast = iter->first.j;

			if (iter->first.j < nFarWest)
				nFarWest = iter->first.j;
		}

		for (int i = nFarNorth; i <= nFarSouth; i++)
		{
			for (int j = nFarWest; j <= nFarEast; j++)
			{
				if (oMatrix.exists(i, j))
					out << oMatrix[i][j];
				else
					out << " ";
			}

			if( i != nFarSouth)
				out << endl;
		}

		return out;
	}

	void clear()
	{
		m_oMap.clear();
	}

	~TwoDDynamicArray(){};
};



#endif /* _TWOD_DYNAMIC_ARRAY_H_ */
