/*
 * Logger.h
 *
 *  Created on: Apr 25, 2016
 *      Author: iliyaaizin 323500942 & yaronlibman 302730072
 */
#ifndef LOGGER_H_
#define LOGGER_H_

#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Logger
{

    static vector<string> vHousesLog;
    static vector<string> vAlgosLog;

public:

    enum class LogType
    {
        Houses, Algos
    };

    static const vector<string>& getLog(LogType oType, bool bSort = false)
    {
        if(oType == LogType::Houses)
        {
            if(bSort)
                std::sort(vHousesLog.begin(), vHousesLog.end());

            return vHousesLog;
        }
        else
        {
            if(bSort)
                std::sort(vAlgosLog.begin(), vAlgosLog.end());

            return vAlgosLog;
        }
    }

    static void addLogMSG(const string &msg, LogType oType)
    {
        if(oType == LogType::Houses)
            vHousesLog.push_back(msg);
        else
            vAlgosLog.push_back(msg);

    }
};

#endif
