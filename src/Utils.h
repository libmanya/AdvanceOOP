#ifndef UTILS_H_
#define UTILS_H_

#include "Direction.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

const string BATTERY_CAPACITY_KEY = "BatteryCapacity";
const string BATTERY_CONSUMPTION_KEY = "BatteryConsumptionRate";
const string BATTERY_RECHARGE_KEY = "BatteryRechargeRate";
const string MAX_STEPS_KEY = "MaxSteps";
const string MAX_STEPS_AFTER_KEY = "MaxStepsAfterWinner";

ostream& operator<<(ostream &out, Direction dir);
Direction reverseDir(Direction dir);
int DirToInt(Direction oDir);
Direction intToDir(int nDir);

struct Point
{
	int i, j;

	Point(){}

	Point(int _i, int _j)
	{
		i = _i;
		j = _j;
	}

	bool operator==(const Point& other) const
	{
		return (i == other.i && j == other.j);
	}

	bool operator!=(const Point& other) const
	{
		return !(*this == other);
	}

	void getNeighbours(vector<Point> &vPoints) const
	{
		vPoints.emplace_back(i - 1, j);
		vPoints.emplace_back(i, j + 1);
		vPoints.emplace_back(i + 1, j);
		vPoints.emplace_back(i, j - 1);
	}
};

Direction direction(Point from, Point to);

Point NeighbourTo(Point &point, Direction oDir);

// ingest Point hash function into std
namespace std
{
	template<> struct hash<Point>
	{
		typedef std::size_t result_type;
		result_type operator()(Point const& s) const
		{
			return (s.i << 5) - s.i + s.j;
		}
	};
}

#endif /* UTILS_H_ */