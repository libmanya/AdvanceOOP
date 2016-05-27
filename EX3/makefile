all: Simulator score_formula A1_.so B2_.so C1_.so basic_smart_.so

clean:
	-rm simulator Sensor.o House.o Simulator.o A1_.so A1.o B2_.so B2.o C1_.so C1.o basic_smart_.so basic_smart.o Utils.o score_formula.o score_formula.so

Simulator: Sensor.o House.o Simulator.o
	g++ -std=c++11 -rdynamic -Wall -pedantic -pthread Simulator.o Sensor.o House.o -o simulator -ldl

Simulator.o: Simulator.cpp Simulator.h Logger.h
	g++ -std=c++11 -fPIC -Wall -O2 -pedantic -pthread -c Simulator.cpp

Sensor.o: Sensor.h AbstractSensor.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c Sensor.cpp

House.o: Direction.h House.h House.cpp Logger.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c House.cpp

A1_.so: Utils.o ./Algorithms/A1.cpp ./Algorithms/A1.h
	g++  -fPIC -c -std=c++11 -O2 -o A1.o ./Algorithms/A1.cpp
	g++  -shared -Wl,-soname,A1.so -o A1_.so A1.o Utils.o

B2_.so: Utils.o ./Algorithms/B2.cpp ./Algorithms/B2.h
	g++  -fPIC -c -std=c++11 -O2 -o B2.o ./Algorithms/B2.cpp
	g++  -shared -Wl,-soname,B2.so -o B2_.so B2.o Utils.o

C1_.so: Utils.o ./Algorithms/C1.cpp ./Algorithms/C1.h
	g++ -fPIC -c -std=c++11 -O2 -o C1.o ./Algorithms/C1.cpp
	g++ -shared -Wl,-soname,C1.so -o C1_.so C1.o Utils.o

basic_smart_.so: Utils.o ./Algorithms/basic_smart.cpp ./Algorithms/basic_smart.h
	g++ -fPIC -c -std=c++11 -O2 -o basic_smart.o ./Algorithms/basic_smart.cpp
	g++ -shared -Wl,-soname,basic_smart_.so -o basic_smart_.so basic_smart.o Utils.o

Utils.o: Direction.h ./Algorithms/Utils.h ./Algorithms/Utils.cpp
		g++ -fPIC -std=c++11 -Wall -O2 -pedantic -pthread -c ./Algorithms/Utils.cpp

score_formula: score_formula.cpp
	g++ -fPIC -c -std=c++11 -o score_formula.o score_formula.cpp
	g++ -shared -Wl,-soname,score_formula.so -o score_formula.so score_formula.o

