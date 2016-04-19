#include <list>
class Logger
{
   public:
      static void addLogInfo(string strMSG); 
	  static void printAllLog(); 
      
   private:
	  static list<string> lsLog;
};