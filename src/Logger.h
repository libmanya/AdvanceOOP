#ifndef LOGGER_H_
#define LOGGER_H_

#include <vector>
#include <string>

using namespace std;

class Logger {

	static vector<string> vLog;

  public:

    static const vector<string>& getLog(){
		return vLog;
	}

    static void addLogMSG(const string &msg){
		vLog.push_back(msg);
	}
};

#endif
