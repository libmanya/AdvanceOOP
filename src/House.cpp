#include "House.h";

House::House(string name, string  Desc, int row, int col, char** Map): m_sHouseName(name), m_sHouseDesc(Desc),m_nRowNumber(row), m_nColNumber(col), m_pMap(Map)
{
	cout << "House Ctor" << endl;
}

void House::PrintHouse()
{
	cout << "printing house" << endl;
	cout << "name: " << m_sHouseName << endl;
	cout << "description: " << m_sHouseDesc << endl;
	cout << "house map:" << endl;

	for (int i = 0; i < m_nRowNumber; i++)
	{
		for (int j = 0; j < m_nColNumber; j++)
		{
			cout << m_pMap[i][j];
		}

		cout << endl;

	}
}