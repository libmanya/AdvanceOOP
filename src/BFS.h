#ifndef __BFS__H_
#define __BFS__H_

#include "TwoDDynamicArray.h"
#include <queue>
#include <unordered_set>

using namespace std;

class BFS 
{
public:
	struct BFSResult
	{
		TDDA<int> oDistances;
		TDDA<Direction> oDirections;
		Point oFoundPoint;
		Point oFrom;

		int getDistance() const
		{
			return oDistances[oFoundPoint];
		}
	};

	class Path
	{
		vector<Direction> m_oPath;
		int m_nStepNumber;

	public:

		Path() :m_nStepNumber(0) {};
		Direction nextStep() { return m_oPath[m_nStepNumber++];}
		bool hasNext() const { return (size_t)m_nStepNumber < m_oPath.size(); }
		void addStep(Direction oDir) { m_oPath.push_back(oDir); }
	};

	template <class T>
	static void run(BFSResult &oResult, const TDDA<T> &oMatrix, const Point &oFrom, const unordered_set<T> &oFirstOf, unordered_set<char> &oLigalChars)
	{
		oResult.oFrom = oFrom;
		oResult.oDistances[oFrom] = 0;
		if (oFirstOf.find(oMatrix[oFrom]) != oFirstOf.cend())
		{
			oResult.oFoundPoint = oFrom;
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
					if(oLigalChars.find(oMatrix[point]) != oLigalChars.end())
					{ 
						oQue.push(point);
					}

					oResult.oDirections[point] = direction(point, oCurrentPoint);
					oResult.oDistances[point] = oResult.oDistances[oCurrentPoint] + 1;

					if (oFirstOf.find(oMatrix[point]) != oFirstOf.cend())
					{
						oResult.oFoundPoint = point;

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
		vector<Direction> vReverseDirs;
		Point point = oResult.oFoundPoint;

		while (point != oResult.oFrom)
		{
			Direction oDir = oResult.oDirections[point];
			vReverseDirs.push_back(oDir);

			point.i += (oDir == Direction::South ? 1 : (oDir == Direction::North ? -1 : 0));
			point.j += (oDir == Direction::East ? 1 : (oDir == Direction::West ? -1 : 0));
		}

		for (int i = vReverseDirs.size() - 1; i >= 0; i--)
			path.addStep(reverseDir(vReverseDirs[i]));
	}
};

#endif