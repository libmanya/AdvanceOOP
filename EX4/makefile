all: Simulator score_formula 323500942_C_.so 323500942_B_.so 323500942_A_.so

clean:
	-rm simulator Sensor.o House.o Simulator.o 323500942_A_.so _323500942_A.o 323500942_B_.so _323500942_B.o 323500942_C_.so _323500942_C.o Utils.o score_formula.o score_formula.so Encoder.o Montage.o

Simulator: Sensor.o Montage.o House.o Encoder.o Simulator.o
	g++ -std=c++11 -rdynamic -Wall -pedantic -pthread Simulator.o Sensor.o Montage.o Encoder.o House.o -o simulator -ldl

Simulator.o: Simulator.cpp Simulator.h Logger.h
	g++ -std=c++11 -fPIC -Wall -O2 -pedantic -pthread -c Simulator.cpp

Sensor.o: Sensor.h AbstractSensor.h
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c Sensor.cpp

House.o: Direction.h House.h House.cpp Logger.h 
		g++ -std=c++11 -Wall -O2 -pedantic -pthread -c House.cpp

Montage.o: Montage.h Montage.cpp
		g++ -fPIC -std=c++11 -Wall -O2 -pedantic -pthread -c Montage.cpp

Encoder.o: Encoder.h Encoder.cpp
		g++ -fPIC -std=c++11 -Wall -O2 -pedantic -pthread -c Encoder.cpp

323500942_A_.so: Utils.o ./Algorithms/_323500942_A.cpp ./Algorithms/_323500942_A.h
	g++  -fPIC -c -std=c++11 -O2 -o _323500942_A.o ./Algorithms/_323500942_A.cpp
	g++  -shared -Wl,-soname,323500942_A_.so -o 323500942_A_.so _323500942_A.o Utils.o

323500942_B_.so: Utils.o ./Algorithms/_323500942_B.cpp ./Algorithms/_323500942_B.h
	g++  -fPIC -c -std=c++11 -O2 -o _323500942_B.o ./Algorithms/_323500942_B.cpp
	g++  -shared -Wl,-soname,323500942_B_.so -o 323500942_B_.so _323500942_B.o Utils.o

323500942_C_.so: Utils.o ./Algorithms/_323500942_C.cpp ./Algorithms/_323500942_C.h
	g++  -fPIC -c -std=c++11 -O2 -o _323500942_C.o ./Algorithms/_323500942_C.cpp
	g++  -shared -Wl,-soname,323500942_C_.so -o 323500942_C_.so _323500942_C.o Utils.o

Utils.o: Direction.h ./Algorithms/Utils.h ./Algorithms/Utils.cpp
		g++ -fPIC -std=c++11 -Wall -O2 -pedantic -pthread -c ./Algorithms/Utils.cpp

score_formula: score_formula.cpp
	g++ -fPIC -c -std=c++11 -o score_formula.o score_formula.cpp
	g++ -shared -Wl,-soname,score_formula.so -o score_formula.so score_formula.o

