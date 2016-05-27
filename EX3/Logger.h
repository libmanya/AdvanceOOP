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
    static vector<string> vScoresLog;

public:

    enum class LogType
    {
        Houses, Algos, Scores
    };

    static const vector<string>& getLog(LogType oType, bool bSort = false)
    {
        if(oType == LogType::Houses)
        {
            if(bSort)
                std::sort(vHousesLog.begin(), vHousesLog.end());

            return vHousesLog;
        }
        else if(oType == LogType::Algos)
        {
            if(bSort)
                std::sort(vAlgosLog.begin(), vAlgosLog.end());

            return vAlgosLog;
        }
        else
        {
            if(bSort)
                std::sort(vScoresLog.begin(), vScoresLog.end());

            return vScoresLog;
        }
    }

    static void addLogMSG(const string &msg, LogType oType)
    {
        if(oType == LogType::Houses)
            vHousesLog.push_back(msg);
        else if(oType == LogType::Algos)
            vAlgosLog.push_back(msg);
        else
        	vScoresLog.push_back(msg);

    }
};

#endif
