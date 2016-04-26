/*
 * NaiveAlgo.cpp
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#include <map>
#include "_323500942_C.h"

Direction _323500942_C::step()
{
	// get sensor information
	SensorInformation oSI = m_pSensor->sense();

	int i = m_oPos.i;
	int j = m_oPos.j;

	if (m_oMatrix.exists(m_oPos) && m_oMatrix[m_oPos] == '~' && oSI.dirtLevel == 0)
		m_nUnexploredOrDustyCellsCount--;

	// update current position dust level
	if(i != 0 || j != 0)
		m_oMatrix[m_oPos] = (char)(oSI.dirtLevel + '0');

	if (m_oMatrix[m_oPos] == '0')
		m_oMatrix[m_oPos] = ' ';

	if (m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
	{
		m_oMatrix[m_oPos]--;

		if (m_oMatrix[m_oPos] == '0')  // if finished cleaning
			m_nUnexploredOrDustyCellsCount--;
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
	case AdvanceToClean:
		oDir = HandleAdvanceToClean();
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

	m_oPos.i += (oDir == Direction::South ? 1 : (oDir == Direction::North ? -1 : 0));
	m_oPos.j += (oDir == Direction::East ? 1 : (oDir == Direction::West ? -1 : 0));

	// update battery level (according to old position)
	if (m_oMatrix[i][j] == 'D')
		m_nBatteryLevel = std::min(m_nBatteryLevel + m_oConfig[BATTERY_RECHARGE_KEY], m_oConfig[BATTERY_CAPACITY_KEY]);
	else
		m_nBatteryLevel = std::max(m_nBatteryLevel - m_oConfig[BATTERY_CONSUMPTION_KEY], 0);

	// update steps to finish
	if (m_bAboutTofinish)
		m_stepsTillFinishing--;

	return oDir;
}

Direction _323500942_C::HandleClean()
{
	// run BFS to calculate distance to D
	BFS::BFSResult result;
	BFS::run(result, m_oMatrix, m_oPos, { 'D' }, m_oNonWallChars);

	if ((m_bAboutTofinish && m_stepsTillFinishing <= result.getDistance() + 1)				// number of steps left is less then number of steps it will take to get back to D + 1
		|| m_nBatteryLevel / m_oConfig[BATTERY_CONSUMPTION_KEY] <= result.getDistance() + 1)// number of steps the battery will hold is less then number of steps it will take to get back to D + 1
	{
		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if (m_oMatrix[m_oPos] == ' ' || m_oMatrix[m_oPos] == '0')
	{
		m_oCurrentState = AlgoState::AdvanceToClean;
		return HandleAdvanceToClean();
	}

	return Direction::Stay;
}

Direction _323500942_C::HandleAdvanceToClean()
{
	// run BFS to calculate distance to D
	BFS::BFSResult result;
	BFS::run(result, m_oMatrix, m_oPos, { 'D' }, m_oNonWallChars);

	if ((m_bAboutTofinish && m_stepsTillFinishing <= result.getDistance() + 1)					// number of steps left is less then number of steps it will take to get back to D + 1
		|| m_nBatteryLevel/m_oConfig[BATTERY_CONSUMPTION_KEY] <= result.getDistance() + 1		// number of steps the battery will hold is less then number of steps it will take to get back to D + 1
		|| m_nUnexploredOrDustyCellsCount == 0)	// finished cleaning
	{
		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if (!m_bIsPathToCleanInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { '~' , '1', '2', '3', '4', '5', '6', '7', '8', '9'}, m_oNonWallChars);

		BFS::getPath(m_oPathToClean, result);

		m_bIsPathToCleanInit = true;
	}

	if (m_oPathToClean.hasNext())
	{
		return m_oPathToClean.nextStep();
	}
	else
	{
		m_bIsPathToCleanInit = false;
		m_oCurrentState = AlgoState::Clean;
		return HandleClean();
	}
	/*if (m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
	{
		m_oCurrentState = AlgoState::Clean;
		return HandleClean();
	}

	// set default previous direction to West (so the first step will be northward)
	if (!m_oPrevStepDirection.exists(m_oPos))
	{
		m_oPrevStepDirection[m_oPos] = Direction::West;
	}

	Direction oPrevStep = m_oPrevStepDirection[m_oPos];

	// move towards first non-wall direction, order: north, east, south, west, north, ...
	int nRealIndex;
	for (int i = DirToInt(oPrevStep) + 1; i <= DirToInt(oPrevStep) + 4; i++)
	{
		nRealIndex = i % 4;
		Direction oDir = intToDir(nRealIndex);
		if (m_oMatrix[NeighbourTo(m_oPos, oDir)] != 'W')
		{
			m_oPrevStepDirection[m_oPos] = oDir;
			return oDir;
		}
	}*/
			
	return Direction::Stay;
}

Direction _323500942_C::HandleAdvanceToD()
{
	if (!m_bIsPathToDInit)
	{
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { 'D' }, m_oNonWallChars);

		BFS::getPath(m_oPathToD, result);

		m_bIsPathToDInit = true;
	}

	if (m_oPathToD.hasNext())
	{
		return m_oPathToD.nextStep();
	}
	else
	{
		m_bIsPathToDInit = false;
		m_oCurrentState = AlgoState::AtD;
		return HandleAtD();
	}
}

Direction _323500942_C::HandleAtD()
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

	m_oCurrentState = AlgoState::AdvanceToClean;
	return HandleAdvanceToClean();
}

Direction _323500942_C::HandleFinish()
{
	return Direction::Stay;
}

void _323500942_C::aboutToFinish(int stepsTillFinishing)
{
	m_bAboutTofinish = true;
	m_stepsTillFinishing = stepsTillFinishing;
}

extern "C" {
AbstractAlgorithm *maker(){
   return new _323500942_C;
}
class proxy { 
public:
   proxy(){
      // register the maker with the factory 
      factory["_323500942_C_"] = maker;
   }
};
// our one instance of the proxy
proxy p;
}
