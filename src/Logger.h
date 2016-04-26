#ifndef LOGGER_H_
#define LOGGER_H_

#include <vector>
#include <string>

using namespace std;

class Logger {

	static vector<string> vLog;

  public:

    static const vector<string>& getLog();

    static void addLogMSG(const string &msg);
};

#endif
