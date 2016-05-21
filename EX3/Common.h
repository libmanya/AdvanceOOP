/*
 * Common.h
 *
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef POINT_H_
#define POINT_H_

#include <vector>
#include <string>

const std::string BATTERY_CAPACITY_KEY = "BatteryCapacity";
const std::string BATTERY_CONSUMPTION_KEY = "BatteryConsumptionRate";
const std::string BATTERY_RECHARGE_KEY = "BatteryRechargeRate";
const std::string MAX_STEPS_KEY = "MaxSteps";
const std::string MAX_STEPS_AFTER_KEY = "MaxStepsAfterWinner";

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

	void getNeighbours(std::vector<Point> &vPoints, bool bIsExtended = false) const
	{
		vPoints.emplace_back(i - 1, j);
		vPoints.emplace_back(i, j + 1);
		vPoints.emplace_back(i + 1, j);
		vPoints.emplace_back(i, j - 1);

		if(bIsExtended)
		{
			vPoints.emplace_back(i - 1, j + 1);
			vPoints.emplace_back(i + 1, j + 1);
			vPoints.emplace_back(i + 1, j - 1);
			vPoints.emplace_back(i - 1, j - 1);
		}
	}
};

#endif /* POINT_H_ */
