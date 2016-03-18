#include "House.h";

House::House(char* name, char*  Desc, int row, int col, char** Map): m_sHouseName(name), m_sHouseDesc(Desc),m_nRowNumber(row), m_nColNumber(col), m_pMap(Map)
{
}

void House::PrintHouse()
{
	//cout << "Only Two arugments allowed" << endl;
}