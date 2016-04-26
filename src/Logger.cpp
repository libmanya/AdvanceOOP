#include "Logger.h"

vector<string> Logger::vLog;

const vector<string>& Logger::getLog()
{
    return vLog;
}

void Logger::addLogMSG(const string &msg)
{
    vLog.push_back(msg);
}
