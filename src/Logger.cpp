#include "Logger.h"

 
static void Logger::addLogInfo(string strMSG)
{
		if(lsLog == null)
		{
			
		}
		
		lsLog.push_back(strMSG);
} 

static void Logger::printAllLog()
{
	for (it=lsLog.begin(); it!=lsLog.end(); ++it)
		cout << ' ' << *it << endl;
}