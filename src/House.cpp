#include "House.h"
#include <stdexcept>

using namespace std;

House::House(const string &sPath)
{
	if(sPath.length() == 0)  // Hard coded default house
	{
		m_sHouseName = "Default house";
		m_sHouseDesc = "Default house foe exercise 1";
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
				m_pMap[i][j] = house1[i][j];
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

	for(int i = 0; i < m_nRowNumber; i++)
		m_pMap[i] = new char[m_nColNumber];

    *this = oFrom;
}

House &House::operator=(const House &oFrom)
{
	if(this == &oFrom)
		return *this;

	m_sHouseName = oFrom.m_sHouseName;
	m_sHouseDesc = oFrom.m_sHouseDesc;

	if(m_nRowNumber != oFrom.m_nRowNumber || m_nColNumber != oFrom.m_nColNumber)
	{
		for(int i = 0; i < m_nRowNumber; i++)
			delete[] m_pMap[i];

		delete[] m_pMap;

		m_pMap = new char*[oFrom.m_nRowNumber];
		for(int i = 0; i < oFrom.m_nRowNumber; i++)
			m_pMap[i] = new char[oFrom.m_nColNumber];
	}

	m_nRowNumber = oFrom.m_nRowNumber;
	m_nColNumber = oFrom.m_nColNumber;

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
			cout << m_pMap[i][j];

		cout << endl;
	}
}
