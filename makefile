all: main

main: main.cpp arithmetic.o command.o interpreter.o interpreter.h
	g++ -g -Wall -std=c++11 arithmetic.o command.o interpreter.o main.cpp -o interpret

interpreter.o: interpreter.h interpreter.cpp command.o
	g++ -g -Wall -c -std=c++11 interpreter.cpp -o interpreter.o

command.o: command.h command.cpp arithmetic.o
	g++ -g -Wall -c -std=c++11 command.cpp -o command.o

arithmetic.o: arithmetic.h arithmetic.cpp
	g++ -g -Wall -c -std=c++11 arithmetic.cpp -o arithmetic.o
