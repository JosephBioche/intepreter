#ifndef INTERPRETER_HPP
#define INTERPRETER_HPP

#include "command.h"

class Interpreter {
public:
    Interpreter(std::istream& in, std::string fileName);
    ~Interpreter();
    void write(std::ostream& out);
    void execute();
    bool read_error();
private:
    void parse(std::istream& in);
    NumericExpression* ParseNumericExpression(std::string line, int& start);
    ConstantExpression* ParseConstant(std::string line, int& start);
    Variable* ParseVariable(std::string line, int& start);
    std::string ParseVariableName(std::string line, int& start);
    StringExpression* ParseString(std::string line, int& start);
    BinaryExpression* ParseBinaryExpression(std::string line, int& start);
    BoolExpression* ParseBoolExpression(std::string line, int& start);
    void ClearWhiteSpace(std::string line, int& start);
    std::string ClearWhiteSpaceString(std::string str);
    VariableArray* VariableArrayExists(std::string name);
    VariableInt* VariableIntExists(std::string name);

    bool zero_error(NumericExpression* x);

    std::vector<Command*>* command_list;
    std::vector<NumericExpression*>* memcheck;
    std::map<std::string, VariableInt*> imap;
    std::multimap<std::string, VariableArray*> amap;
    std::string fileName;

    std::string error;
    std::stack<int> substack;
    bool readError;
};
#endif
