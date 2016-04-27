all: Simulator

clean:
	-rm simulator Sensor.o House.o Simulator.o

Simulator: Sensor.o House.o Simulator.o
	g++ -std=c++11 -rdynamic -Wall -pedantic -pthread Simulator.o Sensor.o House.o -o simulator -ldl

Simulator.o: Simulator.cpp Simulator.h Logger.h
	g++ -std=c++11 -fPIC -Wall -O2 -pedantic -pthread -c Simulator.cpp

Sensor.o: Sensor.h AbstractSensor.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c Sensor.cpp

House.o: Direction.h House.h House.cpp Logger.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c House.cpp







