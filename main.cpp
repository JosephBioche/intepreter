#include <iostream>
#include <fstream>
#include <string>
#include "interpreter/interpreter.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Please provide an input file." << std::endl;
        return 1;
    }
    std::ifstream input(argv[1]);
    if (input.fail()) {
    	std::cout << "File" << argv[1] << "cannot be opened" << std::endl;
    }
    Interpreter interpreter(input, argv[1]);
    if(interpreter.read_error()) {
        return 0;
    }
    interpreter.execute();
    return 0;
}


