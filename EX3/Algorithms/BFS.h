/*
 * BFS.h
 *
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef __BFS__H_
#define __BFS__H_

#include "TwoDDynamicArray.h"
#include <queue>
#include <unordered_set>

using namespace std;

class BFS 
{
public:
	// class that represents a BFS result
	struct BFSResult
	{
		TDDA<int> oDistances;
		TDDA<Direction> oDirections;
		Point oFoundPoint;
		Point oFrom;
		bool bfound = false;

		int getDistance() const
		{
			return oDistances[oFoundPoint];
		}

		void clearResult()
		{

			oDistances.clear();
			oDirections.clear();

			bfound = false;
		}
	};

	// class that represents a path between two point on a TDDA
	class Path
	{
public: // TODO: Delete this
		vector<Direction> m_oPath;
		int m_nStepNumber = 0;

	public:

		Path() :m_nStepNumber(0) {};
		Direction nextStep() { return m_oPath[m_nStepNumber++];}
		bool hasNext() const { return (size_t)m_nStepNumber < m_oPath.size(); }
		void addStep(Direction oDir) { m_oPath.push_back(oDir); }
		void ClearPath() { 	m_nStepNumber = 0; m_oPath.clear();}
		void removeLast() { m_oPath.erase(m_oPath.end() - 1); }

		void addPath(Path &oPathToAdd)
		{
			while(oPathToAdd.hasNext())
				this->addStep(oPathToAdd.nextStep());
		}
	};

	template <class T>
	static bool AdjacentToCheck(const TDDA<T> &oMatrix, const Point &oPoint, const unordered_set<T> &oAdjacentTo)
	{
		if(oAdjacentTo.size() != 0)
		{
			vector<Point> vNeighbours;
			oPoint.getNeighbours(vNeighbours, true);

			for(Point oNeighbour : vNeighbours)
			{
				if(oMatrix.exists(oNeighbour) && oAdjacentTo.find(oMatrix[oNeighbour]) != oAdjacentTo.end())
				{
					return true;
				}
			}

			return false;
		}

		return true;
	}

	/**
	 * oResult - BFS result information
	 * oMatrix - TDDA on witch the search in conducted
	 * oFrom - the starring point of a search
	 * oFirstOf - set of TDDA data that we are looking for, the search will end when some data in oFirstOf is found
	 * oLigalChars - set of data through witch the search can continue
	 */
	template <class T>
	static void run(BFSResult &oResult, const TDDA<T> &oMatrix, const Point &oFrom, const unordered_set<T> &oFirstOf, const unordered_set<T> &oLigalChars, const unordered_set<Point> &oPointsToIgnore = {}, const unordered_set<T> &oAdjacentTo = {})
	{
		oResult.oFrom = oFrom;
		oResult.oDistances[oFrom] = 0;

		// check whether the starting point (oFrom) is already the point we are looking for
		if (oFirstOf.find(oMatrix[oFrom]) != oFirstOf.cend() && oPointsToIgnore.find(oFrom) == oPointsToIgnore.end() && AdjacentToCheck(oMatrix, oFrom, oAdjacentTo))
		{
			oResult.oFoundPoint = oFrom;
			oResult.bfound = true;
			return;
		}

		queue<Point> oQue;
		bool done = false;
		vector<Point> vNeighbours;

		oQue.push(oFrom);

		while (!done && !oQue.empty())
		{
			const Point &oCurrentPoint = oQue.front();

			vNeighbours.clear();
			oCurrentPoint.getNeighbours(vNeighbours);

			for (Point &point : vNeighbours)
			{
				if (oMatrix.exists(point) && !oResult.oDistances.exists(point))
				{
					// add neighbors to queue
					if(oLigalChars.find(oMatrix[point]) != oLigalChars.end())
					{ 
						oQue.push(point);
					}

					// calculate directions and distances
					oResult.oDirections[point] = direction(point, oCurrentPoint);
					oResult.oDistances[point] = oResult.oDistances[oCurrentPoint] + 1;

					// check whether the data was found
					if (oFirstOf.find(oMatrix[point]) != oFirstOf.cend() && oPointsToIgnore.find(point) == oPointsToIgnore.end() && AdjacentToCheck(oMatrix, point, oAdjacentTo))
					{
						oResult.oFoundPoint = point;
						oResult.bfound = true;

						done = true;
						break;
					}
				}
			}

			oQue.pop();
		}
	}

	template <class T>
	static void run(BFSResult &oResult, const TDDA<T> &oMatrix, const Point &oFrom, const Point &oFirstOf, const unordered_set<T> &oLigalChars)
	{
		oResult.oFrom = oFrom;
		oResult.oDistances[oFrom] = 0;

		// check whether the starting point (oFrom) is already the point we are looking for
		if (oFirstOf == oFrom)
		{
			oResult.oFoundPoint = oFrom;
			oResult.bfound = true;
			return;
		}

		queue<Point> oQue;
		bool done = false;
		vector<Point> vNeighbours;

		oQue.push(oFrom);

		while (!done && !oQue.empty())
		{
			const Point &oCurrentPoint = oQue.front();

			vNeighbours.clear();
			oCurrentPoint.getNeighbours(vNeighbours);

			for (Point &point : vNeighbours)
			{
				if (oMatrix.exists(point) && !oResult.oDistances.exists(point))
				{
					// add neighbors to queue
					if(oLigalChars.find(oMatrix[point]) != oLigalChars.end())
					{
						oQue.push(point);
					}

					// calculate directions and distances
					oResult.oDirections[point] = direction(point, oCurrentPoint);
					oResult.oDistances[point] = oResult.oDistances[oCurrentPoint] + 1;

					// check whether the data was found
					if (oFirstOf == point)
					{
						oResult.oFoundPoint = point;
						oResult.bfound = true;

						done = true;
						break;
					}
				}
			}

			oQue.pop();
		}
	}

	static void getPath(Path &path, BFSResult &oResult)
	{
		path.ClearPath();

		if(!oResult.bfound)
		{
			return;
		}

		vector<Direction> vReverseDirs;
		Point point = oResult.oFoundPoint;

		// get path (a vector of directions) from oFoundPoint to oFrom
		while (point != oResult.oFrom)
		{
			Direction oDir = oResult.oDirections[point];
			vReverseDirs.push_back(oDir);

			point.i += (oDir == Direction::South ? 1 : (oDir == Direction::North ? -1 : 0));
			point.j += (oDir == Direction::East ? 1 : (oDir == Direction::West ? -1 : 0));
		}

		// reverse the path
		for (int i = vReverseDirs.size() - 1; i >= 0; i--)
			path.addStep(reverseDir(vReverseDirs[i]));
	}
};

#endif
