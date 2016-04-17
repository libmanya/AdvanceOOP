all: Algo_A_ Algo_B_ Algo_C_

clean:
	-rm Algo_A_.so Algo_A_.o Algo_B_.so Algo_B_.o

Algo_A_:
	g++    -c -std=c++11 -o Algo_A_.o Algo_A_.cpp
	g++ -shared -Wl,-soname,Algo_A_.so -o Algo_A_.so Algo_A_.o

Algo_B_:
	g++    -c -std=c++11 -o Algo_B_.o Algo_B_.cpp
	g++ -shared -Wl,-soname,Algo_B_.so -o Algo_B_.so Algo_B_.o

Algo_C_:
	g++    -c -std=c++11 -o Algo_C_.o Algo_C_.cpp
	g++ -shared -Wl,-soname,Algo_C_.so -o Algo_C_.so Algo_C_.o




