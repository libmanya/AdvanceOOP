all: Simulator A1.so B2.so C1.so basic_smart.so

clean:
	-rm simulator Sensor.o House.o Simulator.o A1.so A1.o B2.so B2.o C1.so C1.o basic_smart.so basic_smart.o Utils.o

Simulator: Sensor.o House.o Simulator.o
	g++ -std=c++11 -rdynamic -Wall -pedantic -pthread Simulator.o Sensor.o House.o -o simulator -ldl

Simulator.o: Simulator.cpp Simulator.h Logger.h
	g++ -std=c++11 -fPIC -Wall -O2 -pedantic -pthread -c Simulator.cpp

Sensor.o: Sensor.h AbstractSensor.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c Sensor.cpp

House.o: Direction.h House.h House.cpp Logger.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c House.cpp

A1.so: Utils.o ./Algorithms/A1.cpp ./Algorithms/A1.h
	g++  -fPIC -c -std=c++11 -o A1.o ./Algorithms/A1.cpp
	g++  -shared -Wl,-soname,A1.so -o A1.so A1.o Utils.o

B2.so: Utils.o ./Algorithms/B2.cpp ./Algorithms/B2.h
	g++  -fPIC -c -std=c++11 -o B2.o ./Algorithms/B2.cpp
	g++  -shared -Wl,-soname,B2.so -o B2.so B2.o Utils.o

C1.so: Utils.o ./Algorithms/C1.cpp ./Algorithms/C1.h
	g++ -fPIC -c -std=c++11 -o C1.o ./Algorithms/C1.cpp
	g++ -shared -Wl,-soname,C1.so -o C1.so C1.o Utils.o

basic_smart.so: Utils.o ./Algorithms/basic_smart.cpp ./Algorithms/basic_smart.h
	g++ -fPIC -c -std=c++11 -o basic_smart.o ./Algorithms/basic_smart.cpp
	g++ -shared -Wl,-soname,basic_smart.so -o basic_smart.so basic_smart.o Utils.o

Utils.o: Direction.h ./Algorithms/Utils.h ./Algorithms/Utils.cpp
		g++ -fPIC -std=c++11 -Wall -O2 -pedantic -pthread -c ./Algorithms/Utils.cpp

score_formula: score_formula.cpp
	g++ -fPIC -c -std=c++11 -o score_formula.o score_formula.cpp
	g++ -shared -Wl,-soname,score_formula.so -o score_formula.so score_formula.o
clean_score_formula:
	-rm score_formula.o score_formula.so





