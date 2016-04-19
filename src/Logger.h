#ifndef LOGGER_H_
#define LOGGER_H_
#include <vector>
#include <string>
class Logger {
private:
  static std::vector<std::string> vcLog;

  public:
    static std::vector<std::string> getLog();

    static void addLogMSG(std::string msg);
};

#endif
