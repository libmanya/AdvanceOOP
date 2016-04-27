all: 323500942_A_.so 323500942_B_.so 323500942_C_.so

clean:
	-rm 323500942_A_.so _323500942_A.o 323500942_B_.so _323500942_B.o 323500942_C_.so _323500942_C.o Utils.o

323500942_A_.so: Utils.o
	g++  -fPIC -c -std=c++11 -o _323500942_A.o _323500942_A.cpp
	g++  -shared -Wl,-soname,323500942_A_.so -o 323500942_A_.so _323500942_A.o Utils.o

323500942_B_.so: Utils.o
	g++  -fPIC -c -std=c++11 -o _323500942_B.o _323500942_B.cpp
	g++  -shared -Wl,-soname,323500942_B_.so -o 323500942_B_.so _323500942_B.o Utils.o

323500942_C_.so: Utils.o
	g++ -fPIC -c -std=c++11 -o _323500942_C.o _323500942_C.cpp
	g++ -shared -Wl,-soname,323500942_C_.so -o 323500942_C_.so _323500942_C.o Utils.o

Utils.o: ../Direction.h Utils.h Utils.cpp
		g++ -fPIC -std=c++11 -Wall -O2 -pedantic -pthread -c Utils.cpp
