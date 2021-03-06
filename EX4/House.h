/*
 * House.h
 *
 *  Created on: Mar 15, 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */
#ifndef HOUSE_H_
#define HOUSE_H_
#include <iostream>
#include <string>
#include "Direction.h"
#include "Logger.h"
#include "Common.h"

using namespace std;

#define WALL_CELL 'W'
#define DOCKING_STATION_CELL 'D'
#define VACUUM_CELL 'R'
#define EMPTY_CELL ' '

const string HOUSES_FILE_SUFFIX = ".house";

class House
{
public:

    House(const string &sFileName,const string &sPath, int nBatteryCapacity, int nBatteryConsumptionRate, int nBatteryRechargeRate);
    House(const House &oFrom);
    House& operator=(const House &oFrom);
    friend ostream& operator<<(ostream& out, const House& oHouse);
    void TryMoveVacuum(Direction oDir);
    bool isVacuumInDocking()const;
    void montage(const string& algoName, const string& houseName);
    void deleteMontageDir(const string& algoName, const string& houseName);

    // Getters
    const Point& GetVacuumPos() 		const
    {
        return m_VacumPos;
    };
    int GetColNumber() 					const
    {
        return m_nColNumber;
    };
    int GetRowNumber() 					const
    {
        return m_nRowNumber;
    };
    const char* operator[](int i)		const
    {
        return m_pMap[i];
    };   // for House[][] support (read only access to sells)
    int GetInitialAmounthOfDirt() 		const
    {
        return m_nInitialAmounthOfDirt;
    };
    int GetDirtCollected() 				const
    {
        return m_nDirtCollected;
    }
    int GetBatteryLevel() 				const
    {
        return m_BatteryLevel;
    };
    const string& GetHouseName()		const
    {
        return m_sHouseName;
    };
    const string& GetHouseFileName()		const
    {
        return m_sHouseFileName;
    };
    int GetMaxSteps() 				const
    {
        return m_nMaxSteps;
    };
    bool isLoadFailed() 				const
    {
        return m_bisLoadFail;
    };
    int numberOfFailedMontage() 		const
    {
        return m_nMontageErrorCounter;
    };
    ~House();

private:

    string m_sHouseName;
    string m_sHouseFileName;
    Point m_VacumPos;
    int m_nRowNumber = 0;
    int m_nColNumber;
    char **m_pMap = nullptr;
    bool m_bisLoadFail;

    //House max Steps
    int m_nMaxSteps;

    // Dirt
    int m_nInitialAmounthOfDirt;
    int m_nDirtCollected = 0;

    // Battery
    int m_BatteryLevel;
    int m_BatteryCapacity;
    int m_BatteryConsumptionRate;
    int m_BatteryRechargeRate;

    int m_nMontageCounter = 0;
    int m_nMontageErrorCounter = 0;

    void TryCollectDirt();
};

#endif /* HOUSE_H_ */
