#include "Logger.h"

std::vector<std::string> Logger::vcLog;
std::vector<std::string> Logger::getLog(){
    return vcLog;
}

void Logger::addLogMSG(std::string msg){
    vcLog.push_back(msg);
}
