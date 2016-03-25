#include "House.h"
#include <stdexcept>

using namespace std;

House::House(const string &sPath, int nBatteryCapacity, int nBatteryConsumptionRate, int nBatteryRechargeRate)
{
	m_nInitialAmounthOfDurt = 0;
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

		char house1[8][10+1] = {
		"wwwwwwwwww",
		"w22  Dw59w",
		"w  w 1119w",
		"w www3ww w",
		"w6   3w  w",
		"w78w  w  w",
		"w99w  w  w",
		"wwwwwwwwww"};

		m_pMap = new char*[m_nRowNumber];
		for(int i = 0; i < m_nRowNumber; i++)
			m_pMap[i] = new char[m_nColNumber];

		for(int i = 0; i < m_nRowNumber; i++)
			for(int j = 0; j < m_nColNumber; j++)
			{
				m_pMap[i][j] = house1[i][j];

				if(house1[i][j] == 'D')
				{
					m_VacumPos.i = i;
					m_VacumPos.j = j;
				}
				else if(house1[i][j] >= '0' && house1[i][j] <= '9')
				{
					m_nInitialAmounthOfDurt += house1[i][j] - '0';
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

	//for(int i = 0; i < m_nRowNumber; i++)
	//	m_pMap[i] = new char[m_nColNumber];

    *this = oFrom;
}

House &House::operator=(const House &oFrom)
{
	if(this == &oFrom)
		return *this;

	m_sHouseName = oFrom.m_sHouseName;
	m_sHouseDesc = oFrom.m_sHouseDesc;
	m_VacumPos = oFrom.m_VacumPos;
	m_nInitialAmounthOfDurt = oFrom.m_nInitialAmounthOfDurt;
	m_nDurtCollected = oFrom.m_nDurtCollected;
	m_BatteryLevel = oFrom.m_BatteryLevel;
	m_BatteryCapacity = oFrom.m_BatteryCapacity;
	m_BatteryConsumptionRate = oFrom.m_BatteryConsumptionRate;
	m_BatteryRechargeRate = oFrom.m_BatteryRechargeRate;

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

void House::PrintHouse() const
{
	cout << "printing house" << endl;
	cout << "name: " << m_sHouseName << endl;
	cout << "description: " << m_sHouseDesc << endl;
	cout << "house map:" << endl;

	for (int i = 0; i < m_nRowNumber; i++)
	{
		for (int j = 0; j < m_nColNumber; j++)
		{
			if(m_VacumPos.i == i && m_VacumPos.j == j)
				cout << 'R';
			else
				cout << m_pMap[i][j];
		}

		cout << endl;
	}
}

void House::TryCollectDirt()
{
	int i = m_VacumPos.i;
	int j = m_VacumPos.j;

	if(m_pMap[i][j] >= '1' && m_pMap[i][j] <= '9')
	{
		m_pMap[i][j]--;
		m_nDurtCollected++;
	}
}

/**
 *  Staying will still result in dirt collection if possible
 */
void House::MoveVacuum(Direction oDir)
{
	// clean if there is dust at current position
	this->TryCollectDirt();

	int i = m_VacumPos.i;
	int j = m_VacumPos.j;

	if(m_pMap[i][j] == DOCKING_STATION_CELL)
		m_BatteryLevel = std::min(m_BatteryLevel + m_BatteryRechargeRate, m_BatteryCapacity);
	else
		m_BatteryLevel = std::max(m_BatteryLevel - m_BatteryConsumptionRate, 0);

	m_VacumPos.i += (oDir == Direction::South ? 1 : (oDir == Direction::North ? -1 : 0));
	m_VacumPos.j += (oDir == Direction::East  ?  1 : (oDir == Direction::West ? -1 : 0));
}
