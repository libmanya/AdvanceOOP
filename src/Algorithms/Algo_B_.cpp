#include "Algo_B_.h"

using namespace std;

void ALGO_B_::setConfiguration(map<string, int> config)
{
	
}
Direction ALGO_B_::step()
{
	return Direction::Stay;
}

void ALGO_B_::aboutToFinish(int stepsTillFinishing)
{
	
	
}
void ALGO_B_::setSensor(const AbstractSensor& sensor)
{

}

extern "C" {
AbstractAlgorithm *maker(){
   return new ALGO_B_;
}
class proxy { 
public:
   proxy(){
      // register the maker with the factory 
      factory.push_back(std::make_pair("ALGO_B_", maker));
   }
};
// our one instance of the proxy
proxy p;
}
