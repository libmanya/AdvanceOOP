all: Simulator

clean:
	-rm simulator Sensor.o House.o NaiveAlgo.o Simulator.o

Simulator: Sensor.o NaiveAlgo.o House.o Simulator.o 
	g++ -std=c++11 -Wall -pedantic -pthread Simulator.o Sensor.o House.o NaiveAlgo.o -o simulator -ldl -rdynamic

Simulator.o: Simulator.cpp Simulator.h Logger.h
	g++ -std=c++11 -Wall -O2 -pedantic -pthread -c Simulator.cpp

Sensor.o: Sensor.h AbstractSensor.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c Sensor.cpp

NaiveAlgo.o: AbstractAlgorithm.h NaiveAlgo.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c NaiveAlgo.cpp

House.o: Direction.h House.h House.cpp Logger.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c House.cpp







