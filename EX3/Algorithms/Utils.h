/*
 * Utils.h
 *
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#ifndef UTILS_H_
#define UTILS_H_

#include "../Direction.h"
#include "../Common.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

ostream& operator<<(ostream &out, Direction dir);
Direction reverseDir(Direction dir);
int DirToInt(Direction oDir);
Direction intToDir(int nDir);
Direction LeftOf(Direction oDir);
Direction RightOf(Direction oDir);
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
