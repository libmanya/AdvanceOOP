all: Simulator

clean:
	-rm simulator Sensor.o House.o NaiveAlgo.o Simulator.o

Simulator: Sensor.o NaiveAlgo.o House.o Simulator.o 
	g++ -std=c++11 -Wall -pedantic -pthread Simulator.o Sensor.o House.o NaiveAlgo.o -o simulator

Simulator.o: Simulator.cpp Simulator.h
	g++ -std=c++11 -Wall -pedantic -pthread -c Simulator.cpp

Sensor.o: Sensor.h AbstractSensor.h
		g++ -std=c++11 -Wall -pedantic -pthread -c Sensor.cpp

NaiveAlgo.o: AbstractAlgorithm.h NaiveAlgo.h
		g++ -std=c++11 -Wall -pedantic -pthread -c NaiveAlgo.cpp

House.o: Direction.h House.h House.cpp
		g++ -std=c++11 -Wall -pedantic -pthread -c House.cpp





