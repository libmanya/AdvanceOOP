/*
* House.cpp
*
*  Created on: Mar 15, 2016
*      Author: iliyaaizin 323500942 & yaronlibman 302730072
*/

#include "House.h"
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "Montage.h"

using namespace std;

House::House(const string &sFileName,const string &sPath, int nBatteryCapacity, int nBatteryConsumptionRate, int nBatteryRechargeRate)
{
    m_nInitialAmounthOfDirt = 0;
    m_BatteryLevel = nBatteryCapacity;
    m_BatteryCapacity = nBatteryCapacity;
    m_BatteryConsumptionRate = nBatteryConsumptionRate;
    m_BatteryRechargeRate = nBatteryRechargeRate;
    m_sHouseFileName = sFileName;
    m_bisLoadFail = false;

    ifstream fin(sPath);

    if(!fin)
    {
        string strError = sFileName + HOUSES_FILE_SUFFIX + ": cannot open file";
        Logger::addLogMSG(strError, Logger::LogType::Houses);
        m_bisLoadFail = true;
        return;
    }

    string sTemp;
    std::getline(fin, m_sHouseName);

    std::getline(fin, sTemp);
    m_nMaxSteps = atoi(sTemp.c_str());

    if(m_nMaxSteps < 1)
    {
        string strError = m_sHouseFileName + HOUSES_FILE_SUFFIX + ": line number 2 in house file shall be a positive number, found: " + to_string(m_nMaxSteps);
        Logger::addLogMSG(strError, Logger::LogType::Houses);
        m_bisLoadFail = true;
        m_nRowNumber = 0; // so not allocated arrays will not be deleted in dector
        return;
    }


    std::getline(fin, sTemp);
    m_nRowNumber = atoi(sTemp.c_str());

    if(m_nRowNumber < 1)
    {
        string strError = m_sHouseFileName + HOUSES_FILE_SUFFIX + ": line number 3 in house file shall be a positive number, found: " + to_string(m_nRowNumber);
        Logger::addLogMSG(strError, Logger::LogType::Houses);
        m_bisLoadFail = true;
        m_nRowNumber = 0; // so not allocated arrays will not be deleted in dector
        return;
    }

    std::getline(fin, sTemp);
    m_nColNumber = atoi(sTemp.c_str());

    if(m_nColNumber < 1)
    {
        string strError = m_sHouseFileName + HOUSES_FILE_SUFFIX + ": line number 4 in house file shall be a positive number, found: " + to_string(m_nColNumber);
        Logger::addLogMSG(strError, Logger::LogType::Houses);
        m_bisLoadFail = true;
        m_nRowNumber = 0;
        return;
    }

    // allocate map
    m_pMap = new char*[m_nRowNumber];
    for(int i = 0; i < m_nRowNumber; i++)
        m_pMap[i] = new char[m_nColNumber];

    int i;
    for(i = 0; i < m_nRowNumber && getline(fin, sTemp); i++)
    {
        int nActualCellsInRow = std::min((int)sTemp.length(), m_nColNumber);
        memcpy(m_pMap[i], sTemp.c_str(), nActualCellsInRow);

        if( nActualCellsInRow < m_nColNumber)
            memset(m_pMap[i] + nActualCellsInRow, EMPTY_CELL, m_nColNumber - nActualCellsInRow);
    }

    // fill missing lines with empty cell
    for(; i < m_nRowNumber; i++)
    {
        memset(m_pMap[i], EMPTY_CELL, m_nColNumber);
    }

    int nDockingCount = 0;
    int nDockingOnPerimeter = 0;

    // process cells
    for(int i = 0; i < m_nRowNumber; i++)
        for(int j = 0; j < m_nColNumber; j++)
        {
            //any case this is an outer wall of the house we set Wall regardless the char from file
            if((i == 0) || (j == 0) || i == (m_nRowNumber - 1) || j == (m_nColNumber - 1))
            {
                if(m_pMap[i][j] == DOCKING_STATION_CELL)
                {
                    nDockingOnPerimeter++;
                }

                m_pMap[i][j] = WALL_CELL;
            }
            else
            {
                if (m_pMap[i][j] == DOCKING_STATION_CELL)
                {
                    m_VacumPos.i = i;
                    m_VacumPos.j = j;
                    nDockingCount++;
                }
                else if (m_pMap[i][j] >= '0' && m_pMap[i][j] <= '9')
                {
                    m_nInitialAmounthOfDirt += m_pMap[i][j] - '0';
                }
                // if the char is not D,W or Number we set Space char (handle worng chars in input)
                else if (m_pMap[i][j] != WALL_CELL)
                {
                    m_pMap[i][j] = EMPTY_CELL;
                }
            }
        }

    if (nDockingCount == 0)
    {
        string strError = m_sHouseFileName + HOUSES_FILE_SUFFIX + ": missing docking station (no D in house)";
        Logger::addLogMSG(strError, Logger::LogType::Houses);
        m_bisLoadFail = true;
        return;
    }
    else if (nDockingCount > 1)
    {
        string strError = m_sHouseFileName + HOUSES_FILE_SUFFIX + ": too many docking stations (more than one D in house)";
        Logger::addLogMSG(strError, Logger::LogType::Houses);
        m_bisLoadFail = true;
        return;
    }
}

House::House(const House &oFrom)
{
    m_nRowNumber = 0;
    m_nColNumber = 0;
    m_nMaxSteps = oFrom.GetMaxSteps();
    m_pMap = nullptr;

    // Construct using operator =
    *this = oFrom;
}

House &House::operator=(const House &oFrom)
{
    if(this == &oFrom)
        return *this;

    m_sHouseName = oFrom.m_sHouseName;
    m_sHouseFileName = oFrom.m_sHouseFileName;
    m_VacumPos = oFrom.m_VacumPos;
    m_nInitialAmounthOfDirt = oFrom.m_nInitialAmounthOfDirt;
    m_nDirtCollected = oFrom.m_nDirtCollected;
    m_BatteryLevel = oFrom.m_BatteryLevel;
    m_BatteryCapacity = oFrom.m_BatteryCapacity;
    m_BatteryConsumptionRate = oFrom.m_BatteryConsumptionRate;
    m_BatteryRechargeRate = oFrom.m_BatteryRechargeRate;
    m_nMaxSteps = oFrom.GetMaxSteps();

    // if dimensions don't agree free current map allocation and reallocate
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
void House::TryMoveVacuum(Direction oDir)
{
    int i = m_VacumPos.i;
    int j = m_VacumPos.j;

    if(m_BatteryLevel > 0 || m_pMap[i][j] == DOCKING_STATION_CELL)
    {
        // clean if there is dust at current position
        this->TryCollectDirt();

        // update position
        m_VacumPos.i += (oDir == Direction::South ? 1 : (oDir == Direction::North ? -1 : 0));
        m_VacumPos.j += (oDir == Direction::East ? 1 : (oDir == Direction::West ? -1 : 0));

        // update battery level
        if (m_pMap[i][j] == DOCKING_STATION_CELL)
            m_BatteryLevel = std::min(m_BatteryLevel + m_BatteryRechargeRate, m_BatteryCapacity);
        else
            m_BatteryLevel = std::max(m_BatteryLevel - m_BatteryConsumptionRate, 0);
    }
}

bool House::isVacuumInDocking()const
{
    return((m_pMap[m_VacumPos.i][m_VacumPos.j]) == DOCKING_STATION_CELL);
}

void createDirectoryIfNotExists(const string& dirPath)
{
  string cmd = "mkdir -p " + dirPath;
  int ret = system(cmd.c_str());
  if (ret == -1)
  {
    //handle error
  }
}

void House::deleteMontageDir(const string& algoName, const string& houseName)
{
  string cmd = "rm -r simulations/" + algoName + "_" + houseName;
  int ret = system(cmd.c_str());
  if (ret == -1)
  {
    //handle error
  }
}

void House::montage(const string& algoName, const string& houseName)
{
        vector<string> tiles;
        for (int row = 0; row < m_nRowNumber; ++row)
        {
          for (int col = 0; col < m_nColNumber; ++col)
          {
            if((row == m_VacumPos.i) &&(col == m_VacumPos.j))
            {
               tiles.push_back("R");
            }
            else if (m_pMap[row][col] == EMPTY_CELL)
            {
              tiles.push_back("0");
            }
            else
            {
              tiles.push_back(string() + m_pMap[row][col]);
            }
          }
        }

        string imagesDirPath = "simulations/" + algoName + "_" + houseName;
        createDirectoryIfNotExists(imagesDirPath);
        string counterStr = to_string(m_nMontageCounter++);
        string composedImage = imagesDirPath + "/image" + string(5-counterStr.length(), '0') + counterStr + ".jpg";
        int result = Montage::compose(tiles, m_nColNumber, m_nRowNumber, composedImage);

        if(result == -1)
        {
            m_nMontageErrorCounter++;
        }
}
