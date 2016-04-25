all: 323500942_A_ 323500942_B_ 323500942_C_ ALGO_A_ ALGO_B_ ALGO_C_

clean:
	-rm 323500942_A_.so _323500942_A.o 323500942_B_.so _323500942_B.o 323500942_C_.so _323500942_C.o ALGO_A_.o ALGO_A_.so ALGO_B_.o ALGO_B_.so ALGO_C_.o ALGO_C_.so

323500942_A_:
	g++    -c -std=c++11 -o _323500942_A.o _323500942_A.cpp
	g++ -shared -Wl,-soname,323500942_A_.so -o 323500942_A_.so _323500942_A.o

323500942_B_:
	g++    -c -std=c++11 -o _323500942_B.o _323500942_B.cpp
	g++ -shared -Wl,-soname,323500942_B_.so -o 323500942_B_.so _323500942_B.o

323500942_C_:
	g++    -c -std=c++11 -o _323500942_C.o _323500942_C.cpp
	g++ -shared -Wl,-soname,323500942_C_.so -o 323500942_C_.so _323500942_C.o

ALGO_A_:
	g++    -c -std=c++11 -o ALGO_A_.o Algo_A_.cpp
	g++ -shared -Wl,-soname,ALGO_A_.so -o ALGO_A_.so ALGO_A_.o
ALGO_B_:
	g++    -c -std=c++11 -o ALGO_B_.o Algo_B_.cpp
	g++ -shared -Wl,-soname,ALGO_B_.so -o ALGO_B_.so ALGO_B_.o
ALGO_C_:
	g++    -c -std=c++11 -o ALGO_C_.o Algo_C_.cpp
	g++ -shared -Wl,-soname,ALGO_C_.so -o ALGO_C_.so ALGO_C_.o




