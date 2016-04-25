#include "Algo_C_.h"

using namespace std;

void ALGO_C_::setConfiguration(map<string, int> config)
{
	
}
Direction ALGO_C_::step()
{
	return Direction::Stay;
}

void ALGO_C_::aboutToFinish(int stepsTillFinishing)
{
	
	
}
void ALGO_C_::setSensor(const AbstractSensor& sensor)
{

}

extern "C" {
AbstractAlgorithm *maker(){
   return new ALGO_C_;
}
class proxy { 
public:
   proxy(){
      // register the maker with the factory 
      factory.push_back(std::make_pair("ALGO_C_", maker));
   }
};
// our one instance of the proxy
proxy p;
}
