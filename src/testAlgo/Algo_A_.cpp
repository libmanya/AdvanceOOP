#include "Algo_A_.h"

using namespace std;


void ALGO_A_::setConfiguration(map<string, int> config){
	
}
Direction ALGO_A_::step(){
	return Direction::Stay;
}

void ALGO_A_::aboutToFinish(int stepsTillFinishing){
	
}
void ALGO_A_::setSensor(const AbstractSensor& sensor){

}


extern "C" {
	AbstractAlgorithm *maker(){
	   return new ALGO_A_;
	}
	class proxy { 
		public:
		   proxy(){
			  // register the maker with the factory 
			  factory["ALGO_A_"] = maker;
		   }
	};
	// our one instance of the proxy
	proxy p;
}
