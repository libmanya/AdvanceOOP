/*
 * Utils.cpp
 *
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#include "Utils.h"

ostream& operator<<(ostream &out, Direction dir)
{
	switch (dir)
	{
	case Direction::North:
		out << 'n';
		break;
	case Direction::East:
		out << 'e';
		break;
	case Direction::South:
		out << 's';
		break;
	case Direction::West:
		out << 'w';
		break;
	case Direction::Stay:
		out << 'S';
		break;
	}
	return out;
}

int DirToInt(Direction oDir)
{
	return ((int) oDir);
}

Direction intToDir(int nDir)
{
	switch (nDir)
	{
	case 0: 
	case 1: 
	case 2: 
	case 3: return ((Direction) nDir);
	default: throw "Illigal direction";
	}
}

Direction reverseDir(Direction dir)
{
	switch (dir)
	{
	case Direction::North:
		return Direction::South;
	case Direction::East:
		return Direction::West;
	case Direction::South:
		return Direction::North;
	case Direction::West:
		return Direction::East;
	case Direction::Stay:
		return Direction::Stay;
	}

	throw "Unreachible code was reached :o";
}

Direction direction(Point from, Point to)
{
	if (to.i == from.i - 1 && to.j == from.j)
		return Direction::North;
	else if (to.i == from.i && to.j == from.j + 1)
		return Direction::East;
	else if (to.i == from.i + 1 && to.j == from.j)
		return Direction::South;
	else if (to.i == from.i && to.j == from.j - 1)
		return Direction::West;
	else
		throw "Two points are not neighbours";
}

Point NeighbourTo(Point &point, Direction oDir)
{
	int i = point.i;
	int j = point.j;

	switch (oDir)
	{
	case Direction::North:
		return { i - 1, j };
	case Direction::East:
		return { i, j + 1 };
	case Direction::South:
		return { i + 1, j };
	case Direction::West:
		return { i , j - 1 };
	case Direction::Stay:
		return point;
	}

	throw "Unreachible code was reached :o";
}
