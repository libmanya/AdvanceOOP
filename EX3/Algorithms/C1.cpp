/*
 * C1.cpp
 *
 *  Created on: May 20, 2016
 *      Author: ilya
 */


#include <map>
#include "C1.h"
#include <cmath>

bool C1::isWallOrUnknown(Point oPoint)
{
	if(m_oMatrix.exists(oPoint))
	{
		if(m_oMatrix[oPoint] == 'W')
			return true;
	}
	else
		return true;

	return false;
}

Direction C1::step(Direction prevStep)
{
	//cout << m_oMatrix << endl;

	m_oPos.i += (prevStep == Direction::South ? 1 : (prevStep == Direction::North ? -1 : 0));
	m_oPos.j += (prevStep == Direction::East ? 1 : (prevStep == Direction::West ? -1 : 0));

	if(prevStep != m_oPrevStep)
	{
		// reenter AdvanceToClean state;

		m_bIsCurrentPathInit = false;		// recalculate path
		//m_oCurrentState = AlgoState::FindWall;

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
			//m_oMatrix[m_oPos] = 'C';
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

	Point prevPos = NeighbourTo(m_oPos, reverseDir(m_oPrevStep));
	if(m_oMatrix[prevPos] == ' ' || m_oMatrix[prevPos] == '0')
	{
		vector<Point> vNeighbours;
		prevPos.getNeighbours(vNeighbours);
		int nWallNeighboursCount = 0;

		for (Point oNeighbour : vNeighbours)
		{
			if(m_oMatrix[oNeighbour] == 'W')
				nWallNeighboursCount++;
		}

		vector<Point> vExtendedNeighbours;
		prevPos.getNeighbours(vExtendedNeighbours, true);
		int nWallExtendedNeighboursCount = 0;

		for (Point oNeighbour : vExtendedNeighbours)
		{
			if(isWallOrUnknown(oNeighbour))
				nWallExtendedNeighboursCount++;
		}

		int nDiagonalNeighbours = nWallExtendedNeighboursCount - nWallNeighboursCount;

		bool isWallable = false;

		int i = prevPos.i;
		int j = prevPos.j;

		/*if(!(isWallOrUnknown({i + 1, j}) && (isWallOrUnknown({i - 1, j - 1}) || isWallOrUnknown({i - 1, j}) || isWallOrUnknown({i - 1, j + 1})))
				&& !(isWallOrUnknown({i, j - 1}) && (isWallOrUnknown({i - 1, j + 1}) || isWallOrUnknown({i, j + 1}) || isWallOrUnknown({i + 1, j + 1})))
				&& !(isWallOrUnknown({i - 1, j}) && (isWallOrUnknown({i + 1, j + 1}) || isWallOrUnknown({i + 1, j - 1})))
				&& !(isWallOrUnknown({i, j + 1}) && (isWallOrUnknown({i - 1, j - 1}) || isWallOrUnknown({i + 1, j - 1}))))
				{
				isWallable = true;
				}*/


		if(nWallNeighboursCount == 0)
		{
			if(nDiagonalNeighbours == 1)
				isWallable = true;
		}
		else if(nWallNeighboursCount == 1)
		{

			if(!(isWallOrUnknown({i + 1, j}) && (isWallOrUnknown({i - 1, j - 1}) || isWallOrUnknown({i - 1, j + 1})))
							&& !(isWallOrUnknown({i, j - 1}) && (isWallOrUnknown({i - 1, j + 1}) || isWallOrUnknown({i + 1, j + 1})))
							&& !(isWallOrUnknown({i - 1, j}) && (isWallOrUnknown({i + 1, j + 1}) || isWallOrUnknown({i + 1, j - 1})))
							&& !(isWallOrUnknown({i, j + 1}) && (isWallOrUnknown({i - 1, j - 1}) || isWallOrUnknown({i + 1, j - 1}))))
			{
				isWallable = true;
			}

		}
		else if(nWallNeighboursCount == 2)
		{
			if((isWallOrUnknown({i, j - 1}) && isWallOrUnknown({i + 1, j}) && !isWallOrUnknown({i - 1, j + 1}))
				|| (isWallOrUnknown({i, j - 1}) && isWallOrUnknown({i - 1, j}) && !isWallOrUnknown({i + 1, j + 1}))
				|| (isWallOrUnknown({i, j + 1}) && isWallOrUnknown({i + 1, j}) && !isWallOrUnknown({i - 1, j - 1}))
				|| (isWallOrUnknown({i - 1, j}) && isWallOrUnknown({i, j + 1}) && !isWallOrUnknown({i + 1, j - 1})))
				{
					isWallable = true;
				}
		}
		else if(nWallNeighboursCount == 3)
		{
			isWallable = true;
		}

		if(isWallable)
			m_oMatrix[prevPos] = 'W';
	}

	// Handle current state
	Direction oDir;
	switch(m_oCurrentState)
	{
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

Direction C1::HandleFindWall()
{
	if (AboutToFinishOrLittleBattery(m_oPos)
		|| m_nUnexploredOrDustyCellsCount == 0)	// finished cleaning
	{
		m_bIsCurrentPathInit = false;

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if(isAdjacentToWall(m_oPos))
	{
		m_bIsCurrentPathInit = false;

		vector<Point> vNeighbours;
		m_oPos.getNeighbours(vNeighbours);

		for(Point oNeighbour : vNeighbours)
		{
			if(m_oMatrix.exists(oNeighbour) && m_oMatrix[oNeighbour] == 'W')
			{
				m_oWallBeingFollowed = oNeighbour;
				break;
			}
		}

		m_bFirstPart = true;

		m_oCurrentState = AlgoState::FollowWall;
		return HandleFollowWall();
	}

	if (!m_bIsCurrentPathInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, {'~'}, m_oNonWallChars);

		if(AboutToFinishOrLittleBattery(result.oFoundPoint, (result.getDistance())*2) && m_oPos != Point(0, 0))
		{
			m_bIsCurrentPathInit = false;

		//	m_oCurrentState = AlgoState::AdvanceToD;
		//	return HandleAdvanceToD();
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

 		return HandleFindWall();
	}

	return Direction::Stay;
}

Direction C1::HandleAdvanceToD()
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

Direction C1::HandleAtD()
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


Direction C1::HandleFollowWall()
{
	if (AboutToFinishOrLittleBattery(m_oPos)
		|| m_nUnexploredOrDustyCellsCount == 0)	// finished cleaning
	{
		m_bIsCurrentPathInit = false;

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	/*if(!m_bFirstStep)
	{
		m_oWallBeingFollowed = NeighbourTo(m_oWallBeingFollowed, m_oPrevStep);
	}
	else
	{
		m_bFirstStep = false;
	}*/

	if(m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
	{
		return Direction::Stay;
	}

	if (!m_bIsCurrentPathInit)
	{

		m_oWallDirection = direction(m_oPos, m_oWallBeingFollowed);
		Direction oRightOf = RightOf(m_oWallDirection);
		Direction oLeftOf = LeftOf(m_oWallDirection);

		if(m_oMatrix[m_oWallBeingFollowed] != 'W')
		{

			m_oWallBeingFollowed = NeighbourTo(m_oWallBeingFollowed, oLeftOf);
			return m_oWallDirection;
		}

		if(m_oMatrix[NeighbourTo(m_oPos, oRightOf)] != 'W')
		{
			Point oTemp = m_oPos;
			m_nSteps2 = 0;

			do
			{
				oTemp = NeighbourTo(oTemp, reverseDir(m_oWallDirection));
				m_nSteps2++;
			}
			while(m_oMatrix.exists(oTemp) && m_oMatrix[oTemp] == '~');
			m_nSteps2--;

			if(m_nSteps2 > 0 && m_oMatrix.exists(oTemp) && m_oMatrix[oTemp] != '~'
					&& m_oMatrix.exists(NeighbourTo(oTemp, oRightOf)) && m_oMatrix[NeighbourTo(oTemp, oRightOf)] != '~')
			{
				m_bIsCurrentPathInit = true;

				m_oCurrentPath.ClearPath();
				for(int i = 0; i < m_nSteps2; i++)
				{
					m_oCurrentPath.addStep(reverseDir(m_oWallDirection));
				}

				m_bFirstPart = true;

			}
			else
			{
				m_oWallBeingFollowed = NeighbourTo(m_oWallBeingFollowed, oRightOf);
				return oRightOf;
			}

		}
		else
		{
			if(m_oMatrix[NeighbourTo(m_oPos, reverseDir(m_oWallDirection))] != 'W')
			{
				m_oWallBeingFollowed = NeighbourTo(m_oPos, oRightOf);
				m_oWallBeingFollowed = NeighbourTo(m_oWallBeingFollowed, reverseDir(m_oWallDirection));

				return reverseDir(m_oWallDirection);
			}
			else
			{
				m_oWallBeingFollowed = NeighbourTo(m_oPos, reverseDir(m_oWallDirection));
				m_oWallBeingFollowed = NeighbourTo(m_oWallBeingFollowed, oLeftOf);

				return LeftOf(m_oWallDirection);
			}
		}
		//}

		/*BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { '~', '1' , '2', '3', '4', '5', '6', '7', '8', '9' }, m_oNonWallChars, { }, {'W'});

		if(!result.bfound)
		{
			m_bIsCurrentPathInit = false;

			m_oCurrentState = AlgoState::AdvanceToD;
			return HandleAdvanceToD();
		}

		BFS::getPath(m_oCurrentPath, result);

		m_bIsCurrentPathInit = true;*/

	}

	if (m_oCurrentPath.hasNext())
	{
		return m_oCurrentPath.nextStep();
	}
	else
	{
		if(m_bFirstPart)
		{
			Direction oRightOf = RightOf(m_oWallDirection);
			//Direction oLeftOf = LeftOf(m_oWallDirection);

			Point oTemp = m_oPos;
			bool isInSecondRow = false;

			for(int i = 0; i < m_nSteps2; i++)
			{
				Point NeighbourToRight = NeighbourTo(oTemp, RightOf(m_oWallDirection));
				if(!isInSecondRow && m_oMatrix[NeighbourToRight] == '~')
				{
					m_oCurrentPath.addStep(RightOf(m_oWallDirection));
					isInSecondRow = true;
				}

				if(isInSecondRow && m_oMatrix[NeighbourTo(NeighbourToRight, m_oWallDirection)] == 'W')
				{
					m_oCurrentPath.addStep(LeftOf(m_oWallDirection));
					isInSecondRow = false;
				}
				m_oCurrentPath.addStep(m_oWallDirection);

				oTemp = NeighbourTo(oTemp, m_oWallDirection);
			}

			if(!isInSecondRow)
			{
				m_oCurrentPath.addStep(RightOf(m_oWallDirection));
				isInSecondRow = true;
			}

			m_oWallBeingFollowed = NeighbourTo(m_oWallBeingFollowed, oRightOf);

			m_bFirstPart = false;
		}
		else
		{
			m_bIsCurrentPathInit = false;

			return HandleFollowWall();
		}

	}

	return Direction::Stay;
}

bool C1::AboutToFinishOrLittleBattery(Point oPos, int nStepsOffset)
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

bool C1::isAdjacentToWall(Point oPoint)
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

Direction C1::HandleFinish()
{
	return Direction::Stay;
}

void C1::aboutToFinish(int stepsTillFinishing)
{
	m_bAboutTofinish = true;
	m_stepsTillFinishing = stepsTillFinishing;
}

extern "C" {
AbstractAlgorithm *maker(){
   return new C1;
}
class proxy { 
public:
   proxy(){
      // register the maker with the factory 
      factory["C1"] = maker;
   }
};
// our one instance of the proxy
proxy p;
}

