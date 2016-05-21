/*
 * B2.cpp
 *
 *  Created on: May 19, 2016
 *      Author: ilya
 */

#include <map>
#include "B2.h"
#include <cmath>

Direction B2::step(Direction prevStep)
{
	//cout << m_oMatrix << endl;

	m_oPos.i += (prevStep == Direction::South ? 1 : (prevStep == Direction::North ? -1 : 0));
	m_oPos.j += (prevStep == Direction::East ? 1 : (prevStep == Direction::West ? -1 : 0));

	if(prevStep != m_oPrevStep)
	{
		// reenter AdvanceToClean state;

		m_bIsCurrentPathInit = false;		// recalculate path
		m_oCurrentState = AlgoState::FindWall;

		m_nMismatchCount++;
	}

	// get sensor information
	SensorInformation oSI = m_pSensor->sense();

	int i = m_oPos.i;
	int j = m_oPos.j;

	if (m_oMatrix.exists(m_oPos) && m_oMatrix[m_oPos] == '~' && oSI.dirtLevel == 0)
		m_nUnexploredOrDustyCellsCount--;

	// update current position dust level
	if(!(i == 0 && j == 0) && m_oMatrix[m_oPos] != 'C')
		m_oMatrix[m_oPos] = (char)(oSI.dirtLevel + '0');

	if (m_oMatrix[m_oPos] == '0')
		m_oMatrix[m_oPos] = ' ';

	if (m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
	{
		m_oMatrix[m_oPos]--;

		if (m_oMatrix[m_oPos] == '0')  // if finished cleaning
		{
			m_oMatrix[m_oPos] = 'C';
			m_nUnexploredOrDustyCellsCount--;
		}

	}

	// update neighbours wall information
	vector<Point> vNeighbours;
	m_oPos.getNeighbours(vNeighbours);

	for (Point point : vNeighbours)
	{
		int nIndex = DirToInt(direction(m_oPos, point));

		if(oSI.isWall[nIndex] == false)
		{
			if (!m_oMatrix.exists(point))
			{
				m_oMatrix[point] = '~';
				m_nUnexploredOrDustyCellsCount++;
			}
		}
		else
		{
			m_oMatrix[point] = 'W';
		}

	}

	// Handle current state
	Direction oDir;
	switch(m_oCurrentState)
	{
	case Clean:
		oDir = HandleClean();
		break;
	case FindWall:
		oDir = HandleFindWall();
		break;
	case FollowWall:
		oDir = HandleFollowWall();
		break;
	case AdvanceToD:
		oDir = HandleAdvanceToD();
		break;
	case AtD:
		oDir = HandleAtD();
		break;
	case Finished:
		oDir = HandleFinish();
		break;
	case ExploreDirtyAria:
		oDir = HandleExploreDirtyAria();
		break;
	}

	m_oPrevStep = oDir;

	// update battery level (according to old position)
	if (m_oMatrix[i][j] == 'D')
		m_nBatteryLevel = std::min(m_nBatteryLevel + m_oConfig[BATTERY_RECHARGE_KEY], m_oConfig[BATTERY_CAPACITY_KEY]);
	else
		m_nBatteryLevel = std::max(m_nBatteryLevel - m_oConfig[BATTERY_CONSUMPTION_KEY], 0);

	// update steps to finish
	if (m_bAboutTofinish)
		m_stepsTillFinishing--;

	m_nSteps++;

	//cout << m_oMatrix << endl;

	return oDir;
}

bool B2::AboutToFinishOrLittleBattery(Point oPos, int nStepsOffset)
{
	// run BFS to calculate distance to D
	BFS::BFSResult result;
	BFS::run(result, m_oMatrix, oPos, { 'D' }, m_oNonWallChars);

	double nMismatchFrequency = ((m_nSteps != 0) ? (((double)m_nMismatchCount) / ((double)m_nSteps)) : 0.0);

	int stepsToGetBack = result.getDistance() + 1;

	stepsToGetBack += ceil(stepsToGetBack*nMismatchFrequency) * 3;  // take in account the fact that mismatches may occur

	if ((m_bAboutTofinish && (m_stepsTillFinishing - nStepsOffset) <= stepsToGetBack)				// number of steps left is less then number of steps it will take to get back to D + 1
		|| (m_nBatteryLevel - nStepsOffset*m_oConfig[BATTERY_CONSUMPTION_KEY]) / m_oConfig[BATTERY_CONSUMPTION_KEY] <= stepsToGetBack)// number of steps the battery will hold is less then number of steps it will take to get back to D + 1
		return true;

	return false;
}

Direction B2::HandleClean()
{
	if (AboutToFinishOrLittleBattery(m_oPos))
	{
		m_bIsCurrentPathInit = false;
		m_vFoundDirtyCells.clear();

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if(!m_bIsCurrentPathInit)
	{
		Point oSearchPoint = m_oPos;
		unordered_set<Point> oPointsToIgnore;
		m_oCurrentPath.ClearPath();
		oVisitNumber.clear();

		do
		{
			BFS::BFSResult result;
			BFS::run(result, m_oMatrix, oSearchPoint, { '1', '2', '3', '4', '5', '6', '7', '8', '9'}, { '1', '2', '3', '4', '5', '6', '7', '8', '9'}, oPointsToIgnore);

			if(!result.bfound)
			{
				break;
			}
			else
			{
				BFS::Path oTempPath;
				BFS::getPath(oTempPath, result);
				m_oCurrentPath.addPath(oTempPath);

				oSearchPoint = result.oFoundPoint;

				oPointsToIgnore.insert(result.oFoundPoint);
			}
		}
		while(true);

		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, oSearchPoint, {'~'}, { '1', '2', '3', '4', '5', '6', '7', '8', '9'});

		if(result.bfound)
		{
			BFS::Path oTempPath;
			BFS::getPath(oTempPath, result);
			//oTempPath.removeLast();   // dont step into the tilda
			m_oCurrentPath.addPath(oTempPath);
		}
		else
		{

			BFS::BFSResult result;
			BFS::run(result, m_oMatrix, oSearchPoint, {'~'}, m_oNonWallChars);

			if(result.bfound)
			{
				int nDist = INT32_MAX;
				BFS::Path oTempPath;
				Point oFrom = oSearchPoint;

				for(Point oPoint : m_vFoundDirtyCells)
				{
					BFS::BFSResult result;
					BFS::run(result, m_oMatrix, oPoint, {'~'}, m_oNonWallChars);

					if(result.getDistance() < nDist)
					{
						BFS::getPath(oTempPath, result);
						oFrom = oPoint;
						nDist = result.getDistance();
					}
				}

				BFS::BFSResult result;
				BFS::run(result, m_oMatrix, oSearchPoint, oFrom, { '1', '2', '3', '4', '5', '6', '7', '8', '9'});

				BFS::Path oTempPath2;
				BFS::getPath(oTempPath2, result);
				m_oCurrentPath.addPath(oTempPath2);

				oTempPath.removeLast();   // dont step into the tilda
				m_oCurrentPath.addPath(oTempPath);
			}
			else
			{
				/*BFS::BFSResult result;
				BFS::Path oTempPath;
				BFS::run(result, m_oMatrix, oSearchPoint, {'D'}, m_oNonWallChars);

				BFS::getPath(oTempPath, result);
				oTempPath.removeLast();   // dont step into the D
				m_oCurrentPath.addPath(oTempPath);*/
			}

		}

		BFS::Path oTempPath = m_oCurrentPath;
		Point oCurrentPoint = m_oPos;
		oVisitNumber[oCurrentPoint] = 1;

		while(oTempPath.hasNext())
		{
			Direction oNextDir = oTempPath.nextStep();

			oCurrentPoint.i += (oNextDir == Direction::South ? 1 : (oNextDir == Direction::North ? -1 : 0));
			oCurrentPoint.j += (oNextDir == Direction::East ? 1 : (oNextDir == Direction::West ? -1 : 0));

			if(oVisitNumber.find(oCurrentPoint) != oVisitNumber.end())
				oVisitNumber[oCurrentPoint]++;
			else
				oVisitNumber[oCurrentPoint] = 1;
		}


		m_bIsCurrentPathInit = true;
	}

	if (m_oCurrentPath.hasNext())
	{
		if(m_oMatrix[m_oPos] <= (oVisitNumber[m_oPos] - 1 + '0') || m_oMatrix[m_oPos] == 'C')
		{
			(oVisitNumber[m_oPos])--;
			return m_oCurrentPath.nextStep();
		}
		else
		{
			return Direction::Stay;
		}
	}
	else
	{
		m_bIsCurrentPathInit = false;
		m_vFoundDirtyCells.clear();

		if((m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9') || m_oMatrix[m_oPos] == 'C')
		{
			m_vFoundDirtyCells.push_back(m_oPos);
			m_oCurrentState = AlgoState::ExploreDirtyAria;
			return HandleExploreDirtyAria();
		}

		m_oCurrentState = AlgoState::FindWall;
		return HandleFindWall();

	}

	return Direction::Stay;
}

bool B2::oneLineDirtyAria(const vector<Point> &vFoundDirtyCells)
{
	Point oFirst = vFoundDirtyCells[0];
	Point oSecond;
	bool bIsHorisontal;
	bool bIsVerticle;

	if(vFoundDirtyCells.size() > 1)
		oSecond = vFoundDirtyCells[1];
	else
		return true;

	if(oFirst.i == oSecond.i)
		bIsHorisontal = true;
	else if(oFirst.j == oSecond.j)
		bIsVerticle = true;
	else
		return false;

	for(size_t i = 2; i < vFoundDirtyCells.size(); i++)
	{
		if((bIsHorisontal && vFoundDirtyCells[i].i != oFirst.i)
				|| (!bIsVerticle && vFoundDirtyCells[i].j != oFirst.j))
			return false;
	}

	return true;
}

Direction B2::HandleExploreDirtyAria()
{
	if (AboutToFinishOrLittleBattery(m_oPos))
	{
		m_bIsCurrentPathInit = false;
		m_vFoundDirtyCells.clear();

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if(!m_bIsCurrentPathInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { '~' }, {'1', '2', '3', '4', '5', '6', '7', '8', '9'});

		if(!result.bfound)
		{
			m_bIsCurrentPathInit = false;
			//m_vFoundDirtyCells.clear();

			m_oCurrentState = AlgoState::Clean;
			return HandleClean();
		}


		BFS::BFSResult oSecondResult;
		BFS::run(oSecondResult, m_oMatrix, m_oPos, { '~' }, {'1', '2', '3', '4', '5', '6', '7', '8', '9'}, {result.oFoundPoint});

		if(!oSecondResult.bfound && oneLineDirtyAria(m_vFoundDirtyCells))
		{
			m_bIsCurrentPathInit = false;
			//m_vFoundDirtyCells.clear();

			m_oCurrentState = AlgoState::Clean;
			return HandleClean();
		}

		BFS::getPath(m_oCurrentPath, result);

		m_bIsCurrentPathInit = true;
	}

	if (m_oCurrentPath.hasNext())
	{
		return m_oCurrentPath.nextStep();
	}
	else
	{
		m_bIsCurrentPathInit = false;

		if(m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
		{
			m_vFoundDirtyCells.push_back(m_oPos);
			return HandleExploreDirtyAria();
		}
		else
		{
			m_oLastDiscoveredDirtyAriaBorder = m_oPos;
			return reverseDir(m_oPrevStep);
		}

	}

	return Direction::Stay;
}

Direction B2::HandleAdvanceToD()
{
	if (!m_bIsCurrentPathInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { 'D' }, m_oNonWallChars);

		BFS::getPath(m_oCurrentPath, result);

		m_bIsCurrentPathInit = true;
	}

	if (m_oCurrentPath.hasNext())
	{
		return m_oCurrentPath.nextStep();
	}
	else
	{
		m_bIsCurrentPathInit = false;
		m_oCurrentState = AlgoState::AtD;
		return HandleAtD();
	}
}

bool B2::isAdjacentToWall(Point oPoint)
{
	// check whether adjacent to wall
	vector<Point> vNeighbours;
	oPoint.getNeighbours(vNeighbours);

	for(Point oNeighbour : vNeighbours)
	{
		if(m_oMatrix.exists(oNeighbour) && m_oMatrix[oNeighbour] == 'W')
		{
			return true;
		}
	}

	return false;
}

Direction B2::HandleFindWall()
{
	if (AboutToFinishOrLittleBattery(m_oPos)
		|| m_nUnexploredOrDustyCellsCount == 0)	// finished cleaning
	{
		m_bIsCurrentPathInit = false;

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	BFS::BFSResult result;
	BFS::run(result, m_oMatrix, m_oPos, { '~', '1' , '2', '3', '4', '5', '6', '7', '8', '9'}, m_oNonWallChars, { }, {'W'});

	if(result.bfound)
	{
		m_bIsCurrentPathInit = false;
		m_oCurrentState = AlgoState::FollowWall;
		return HandleFollowWall();
	}

	if (!m_bIsCurrentPathInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, {'1', '2', '3', '4', '5', '6', '7', '8', '9'}, m_oNonWallChars);

		if(!result.bfound)
		{
			result.clearResult();

			BFS::run(result, m_oMatrix, m_oPos, {'~'}, m_oNonWallChars);

		}

		if(AboutToFinishOrLittleBattery(result.oFoundPoint, (result.getDistance())*2) && m_oPos != Point(0, 0))
		{
			m_bIsCurrentPathInit = false;

			m_oCurrentState = AlgoState::AdvanceToD;
			return HandleAdvanceToD();
		}

		BFS::getPath(m_oCurrentPath, result);

		m_bIsCurrentPathInit = true;
	}

	if (m_oCurrentPath.hasNext())
	{
		return m_oCurrentPath.nextStep();
	}
	else
	{
		m_bIsCurrentPathInit = false;

		if(m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
		{
			m_vFoundDirtyCells.push_back(m_oPos);
			m_oCurrentState = AlgoState::ExploreDirtyAria;
			return HandleExploreDirtyAria();
		}
 		else
 		{
 			return HandleFindWall();
 		}
	}

	return Direction::Stay;
}

Direction B2::HandleFollowWall()
{
	if (AboutToFinishOrLittleBattery(m_oPos)
		|| m_nUnexploredOrDustyCellsCount == 0)	// finished cleaning
	{
		m_bIsCurrentPathInit = false;

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if(m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
	{
		m_bIsCurrentPathInit = false;

		m_vFoundDirtyCells.push_back(m_oPos);

		m_oCurrentState = AlgoState::ExploreDirtyAria;
		return HandleExploreDirtyAria();
	}

	if (!m_bIsCurrentPathInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { '~', '1' , '2', '3', '4', '5', '6', '7', '8', '9' }, m_oNonWallChars, { }, {'W'});

		if(!result.bfound)
		{
			m_bIsCurrentPathInit = false;

			m_oCurrentState = AlgoState::AdvanceToD;
			return HandleAdvanceToD();
		}

		BFS::getPath(m_oCurrentPath, result);

		m_bIsCurrentPathInit = true;
	}

	if (m_oCurrentPath.hasNext())
	{
		return m_oCurrentPath.nextStep();
	}
	else
	{
		m_bIsCurrentPathInit = false;

		return HandleFollowWall();
	}

	return Direction::Stay;
}

Direction B2::HandleAtD()
{
	// checked if finished counting
	if (m_nUnexploredOrDustyCellsCount == 0)
	{
		m_oCurrentState = AlgoState::Finished;
		return HandleFinish();
	}

	// if about to finish
	if( m_bAboutTofinish && m_stepsTillFinishing <= 1)
		return Direction::Stay;

	// if not charged yet - stay
	if(m_nBatteryLevel + m_oConfig[BATTERY_RECHARGE_KEY] < m_oConfig[BATTERY_CAPACITY_KEY])
		return Direction::Stay;

	m_oCurrentState = AlgoState::FindWall;
	return HandleFindWall();
}

Direction B2::HandleFinish()
{
	return Direction::Stay;
}

void B2::aboutToFinish(int stepsTillFinishing)
{
	m_bAboutTofinish = true;
	m_stepsTillFinishing = stepsTillFinishing;
}

extern "C" {
AbstractAlgorithm *maker(){
   return new B2;
}
class proxy { 
public:
   proxy(){
      // register the maker with the factory 
      factory["B2"] = maker;
   }
};
// our one instance of the proxy
proxy p;
}

