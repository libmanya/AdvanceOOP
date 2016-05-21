/*
 * A1.cpp
 *
 *  Created on: 19 Mar 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */

#include <map>
#include "A1.h"
#include <cmath>

Direction A1::step(Direction prevStep)
{
	//cout << m_oMatrix << endl;

	m_oPos.i += (prevStep == Direction::South ? 1 : (prevStep == Direction::North ? -1 : 0));
	m_oPos.j += (prevStep == Direction::East ? 1 : (prevStep == Direction::West ? -1 : 0));

	if(prevStep != m_oPrevStep)
	{
		// reenter AdvanceToClean state;

		m_bIsCurrentPathInit = false;		// recalculate path
		m_oCurrentState = AlgoState::AdvanceToClean;

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

bool A1::AboutToFinishOrLittleBattery(Point oPos, int nStepsOffset)
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

Direction A1::HandleClean()
{
	if (AboutToFinishOrLittleBattery(m_oPos))
	{
		m_bIsCurrentPathInit = false;

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	/*if (m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
	{
		return Direction::Stay;
	}*/


	if(!m_bIsCurrentPathInit)
	{
		/*
		BFS::BFSResult result;
		BFS::run(result, m_oMatrix, m_oPos, { '1', '2', '3', '4', '5', '6', '7', '8', '9' }, {'C'});

		if(!result.bfound)
		{
			m_bIsCurrentPathInit = false;
			m_oCurrentState = AlgoState::AdvanceToClean;
			return HandleAdvanceToClean();
		}

		BFS::getPath(m_oCurrentPath, result);

		m_bIsCurrentPathInit = true;
		*/

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
			oTempPath.removeLast();   // dont step into the tilda
			m_oCurrentPath.addPath(oTempPath);
		}
		else
		{
			//if(m_nUnexploredOrDustyCellsCount - m_oMatrix[m_oPos] > 0)
			//{
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



/*
			BFS::BFSResult result;
			BFS::run(result, m_oMatrix, oSearchPoint, {'~'}, m_oNonWallChars);
			if(result.bfound)
			{
				BFS::Path oTempPath;
				BFS::getPath(oTempPath, result);
				oTempPath.removeLast();   // dont step into the tilda
				m_oCurrentPath.addPath(oTempPath);
			}*/

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

			//cout << oCurrentPoint.i << " " << oCurrentPoint.j << " " << oVisitNumber[oCurrentPoint] << endl;
		}


		m_bIsCurrentPathInit = true;
	}

	if (m_oCurrentPath.hasNext())
	{
		//cout << m_oPos.i << " " << m_oPos.j << " " << oVisitNumber[m_oPos] << endl;
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
		if(m_oMatrix[m_oPos] >= '1' && m_oMatrix[m_oPos] <= '9')
			return Direction::Stay;

		m_bIsCurrentPathInit = false;
		m_vFoundDirtyCells.clear();

		m_oCurrentState = AlgoState::AdvanceToClean;
		return HandleAdvanceToClean();

	}

	return Direction::Stay;
}

bool oneLineDirtyAria(const vector<Point> &vFoundDirtyCells)
{
	Point oFirst = vFoundDirtyCells[0];
	Point oSecond;
	bool bIsHorisontal;

	if(vFoundDirtyCells.size() > 1)
		oSecond = vFoundDirtyCells[1];

	if(oFirst.i == oSecond.i)
		bIsHorisontal = true;
	else
		bIsHorisontal= false;

	for(size_t i = 2; i < vFoundDirtyCells.size(); i++)
	{
		if((bIsHorisontal && vFoundDirtyCells[i].i != oFirst.i)
				|| (!bIsHorisontal && vFoundDirtyCells[i].j != oFirst.j))
			return false;
	}

	return true;
}

Direction A1::HandleExploreDirtyAria()
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

Direction A1::HandleAdvanceToClean()
{
	if (AboutToFinishOrLittleBattery(m_oPos)
		|| m_nUnexploredOrDustyCellsCount == 0)	// finished cleaning
	{
		m_bIsCurrentPathInit = false;

		m_oCurrentState = AlgoState::AdvanceToD;
		return HandleAdvanceToD();
	}

	if (!m_bIsCurrentPathInit)
	{
		/*vector<Point> vNeighbours;
		m_oPos.getNeighbours(vNeighbours);

		int nMaxDiscoveredCount = 0;
		Direction oMaxDiscoveredDirection = Direction::Stay;

		for(Point oNeighbour : vNeighbours)
		{
			if(m_oMatrix[oNeighbour] != '~')
				continue;

			vector<Point> vNeighboursNeighbours;
			oNeighbour.getNeighbours(vNeighboursNeighbours);

			int nNonTildaCount = 0;
			for(Point oNeighboursNeighbour : vNeighboursNeighbours)
			{
				if(m_oMatrix.exists(oNeighboursNeighbour) && m_oMatrix[oNeighboursNeighbour] != '~')
					nNonTildaCount++;
			}

			if(nNonTildaCount > nMaxDiscoveredCount)
			{
				nMaxDiscoveredCount = nNonTildaCount;
				oMaxDiscoveredDirection = direction(m_oPos, oNeighbour);
			}

		}

		if(nMaxDiscoveredCount > 0)
		{
			m_oCurrentPath.ClearPath();
			m_oCurrentPath.addStep(oMaxDiscoveredDirection);
		}

		else
		{*/
			BFS::BFSResult result;
			BFS::run(result, m_oMatrix, m_oPos, { '~' , '1', '2', '3', '4', '5', '6', '7', '8', '9'}, m_oNonWallChars);

			if(AboutToFinishOrLittleBattery(result.oFoundPoint, (result.getDistance())*2) && m_oPos != Point(0, 0))
			{
				m_bIsCurrentPathInit = false;

				m_oCurrentState = AlgoState::AdvanceToD;
				return HandleAdvanceToD();
			}

			BFS::getPath(m_oCurrentPath, result);
		//}

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
			return HandleAdvanceToClean();
		}
	}
			
	return Direction::Stay;
}

Direction A1::HandleAdvanceToD()
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

Direction A1::HandleAtD()
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

Direction A1::HandleFinish()
{
	return Direction::Stay;
}

void A1::aboutToFinish(int stepsTillFinishing)
{
	m_bAboutTofinish = true;
	m_stepsTillFinishing = stepsTillFinishing;
}

extern "C" {
AbstractAlgorithm *maker(){
   return new A1;
}
class proxy { 
public:
   proxy(){
      // register the maker with the factory 
      factory["A1"] = maker;
   }
};
// our one instance of the proxy
proxy p;
}

