#include <iostream> 
#include "circle.h"
using namespace std;

void ALGO_A_::setSensor(const AbstractSensor& sensor){
	
}
void ALGO_A_::setConfiguration(map<string, int> config){
	
}
Direction ALGO_A_::step(){
	
}
void ALGO_A_::aboutToFinish(int stepsTillFinishing){
	
}
void ALGO_A_::~AbstractAlgorithm(){
	
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
