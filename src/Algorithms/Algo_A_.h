#ifndef __ALGO_A_H 
#define __ALGO_A_H
#include "AbstractAlgorithm.h"
#include "ExternalAlgo.h"
class ALGO_A_ : public AbstractAlgorithm {
	public:
		void setSensor(const AbstractSensor& sensor) = 0; 
		
		// setConfiguration is called once when the Algorithm is initialized - see below 
		void setConfiguration(map<string, int> config) = 0; 
		
		// step is called by the simulation for each time unit 
		Direction step() = 0; 
		
		// this method is called by the simulation either when there is a winner or 
		// when steps == MaxSteps - MaxStepsAfterWinner 
		// parameter stepsTillFinishing == MaxStepsAfterWinner 
		void aboutToFinish(int stepsTillFinishing) = 0; 

		~AbstractAlgorithm(){};
};
#endif // __ALGO_A_H