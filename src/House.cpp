#include "House.h"
#include <stdexcept>
#include <algorithm>

using namespace std;

House::House(const string &sPath, int nBatteryCapacity, int nBatteryConsumptionRate, int nBatteryRechargeRate)
{
	m_nInitialAmounthOfDirt = 0;
	m_BatteryLevel = nBatteryCapacity;
	m_BatteryCapacity = nBatteryCapacity;
	m_BatteryConsumptionRate = nBatteryConsumptionRate;
	m_BatteryRechargeRate = nBatteryRechargeRate;

	if(sPath.length() == 0)  // Hard coded default house
	{
		m_sHouseName = "Default house";
		m_sHouseDesc = "Default house for exercise 1";
		m_nRowNumber = 8;
		m_nColNumber = 10;

		// hard-codded house
		char house1[8][10+1] = {
		"wwwwwwwwww",
		"w22  Dw59w",
		"w  w 1119w",
		"w www3ww w",
		"w6   3w  w",
		"w78w  w  w",
		"w99w  w  w",
		"wwwwwwwwww"};

		// allocate map
		m_pMap = new char*[m_nRowNumber];
		for(int i = 0; i < m_nRowNumber; i++)
			m_pMap[i] = new char[m_nColNumber];

		// process cells
		for(int i = 0; i < m_nRowNumber; i++)
			for(int j = 0; j < m_nColNumber; j++)
			{
				//any case this is an outer wall of the house we set Wall regardless the char from file
				if((i == 0) || (j == 0) || i == (m_nRowNumber - 1) || j == (m_nColNumber - 1))
				{
					m_pMap[i][j] = WALL_CELL;
				}
				else 
				{
					m_pMap[i][j] = house1[i][j];

					if (house1[i][j] == DOCKING_STATION_CELL)
					{
						m_VacumPos.i = i;
						m_VacumPos.j = j;
					}
					else if (house1[i][j] >= '0' && house1[i][j] <= '9')
					{
						m_nInitialAmounthOfDirt += house1[i][j] - '0';
					}
					// if the char is not D,W or Number we set Space char (handle worng chars in input)
					else if (m_pMap[i][j] != WALL_CELL)
					{
						m_pMap[i][j] = EMPTY_CELL;
					}
				}
			}
	}
	else
	{
		throw new invalid_argument("Loading house from actual file is not supported in exercise 1");
	}
}

House::House(const House &oFrom)
{
	m_nRowNumber = 0;
	m_nColNumber = 0;

	m_pMap = nullptr;

	// constract using operator =
    *this = oFrom;
}

House &House::operator=(const House &oFrom)
{
	if(this == &oFrom)
		return *this;

	m_sHouseName = oFrom.m_sHouseName;
	m_sHouseDesc = oFrom.m_sHouseDesc;
	m_VacumPos = oFrom.m_VacumPos;
	m_nInitialAmounthOfDirt = oFrom.m_nInitialAmounthOfDirt;
	m_nDirtCollected = oFrom.m_nDirtCollected;
	m_BatteryLevel = oFrom.m_BatteryLevel;
	m_BatteryCapacity = oFrom.m_BatteryCapacity;
	m_BatteryConsumptionRate = oFrom.m_BatteryConsumptionRate;
	m_BatteryRechargeRate = oFrom.m_BatteryRechargeRate;

	// if dimentions don't agree free current map allocation and reallocate
	if(m_nRowNumber != oFrom.m_nRowNumber || m_nColNumber != oFrom.m_nColNumber)
	{
		for(int i = 0; i < m_nRowNumber; i++)
			delete[] m_pMap[i];

		delete[] m_pMap;

		m_pMap = new char*[oFrom.m_nRowNumber];
		for(int i = 0; i < oFrom.m_nRowNumber; i++)
			m_pMap[i] = new char[oFrom.m_nColNumber];

		m_nRowNumber = oFrom.m_nRowNumber;
		m_nColNumber = oFrom.m_nColNumber;
	}

	// copy map cells
	for(int i = 0; i < m_nRowNumber; i++)
		for(int j = 0; j < m_nColNumber; j++)
			m_pMap[i][j] = oFrom.m_pMap[i][j];

	return *this;
}

House::~House()
{
	for(int i = 0; i < m_nRowNumber; i++)
		delete[] m_pMap[i];

	delete[] m_pMap;
}

// Print the house to stream
ostream& operator<<(ostream& out, const House& oHouse)
{
	out << "=== Printing house ===" << endl;
	out << "	Name: " << oHouse.m_sHouseName << endl;
	out << "	Description: " << oHouse.m_sHouseDesc << endl;
	out << "	House map:" << endl;

	for (int i = 0; i < oHouse.m_nRowNumber; i++)
	{
		out << '\t';

		for (int j = 0; j < oHouse.m_nColNumber; j++)
		{
			if (oHouse.m_VacumPos.i == i && oHouse.m_VacumPos.j == j)
				out << 'R';
			else
				out << oHouse.m_pMap[i][j];
		}

		out << endl;
	}

	return out;
}

// If there is dirt at the current position its level will decrease by 1
void House::TryCollectDirt()
{
	int i = m_VacumPos.i;
	int j = m_VacumPos.j;

	if(m_pMap[i][j] >= '1' && m_pMap[i][j] <= '9')
	{
		m_pMap[i][j]--;
		m_nDirtCollected++;
	}
}

// Staying will still result in dirt collection if possible
void House::MoveVacuum(Direction oDir)
{
	// clean if there is dust at current position
	this->TryCollectDirt();

	int i = m_VacumPos.i;
	int j = m_VacumPos.j;

	// update battery level
	if(m_pMap[i][j] == DOCKING_STATION_CELL)
		m_BatteryLevel = std::min(m_BatteryLevel + m_BatteryRechargeRate, m_BatteryCapacity);
	else
		m_BatteryLevel = std::max(m_BatteryLevel - m_BatteryConsumptionRate, 0);

	// update position
	m_VacumPos.i += (oDir == Direction::South ? 1 : (oDir == Direction::North ? -1 : 0));
	m_VacumPos.j += (oDir == Direction::East  ?  1 : (oDir == Direction::West ? -1 : 0));
}
