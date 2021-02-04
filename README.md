Interpreter is a program that employs complex inheritance structures to parse and execute simple basic commands. Three files demonstrate the capabilities of this program:
	+ embeddedBinary - initializing values in both simple variables as well as arrays, 
		and using those values to perform calculations
	+ goSubRealDeep - jumping lines of code and returning 
	+ youCantDivByZero - demonstrates the ability of the program to detect and prevent division by 0

To compile:
	make
To run: 
	./interpret embeddedBinary.in
	./interpret goSubRealDeep.in
	./interpret youCantDivByZero.in

Note: designed for C++11
