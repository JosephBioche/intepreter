#ifndef COMMAND_H
#define COMMAND_H

#include "arithmetic.h"
#include <vector>
#include <utility>
#include <map>
#include <stack>
//----------------------------------------------------------------------------//
class Command {
public:
	Command(int lineNumber) : lineNumber(lineNumber) {};
	virtual ~Command() {};
    virtual std::string format() const = 0;
	virtual std::string execute(int &start) = 0;
	int cfind(int j, std::vector<Command*>* list, int start, int end);
	int lineNumber;
};

class Print : public Command {
public:
	Print(int lineNumber, NumericExpression* print) : Command(lineNumber), 
	print(print) {};
	virtual ~Print() {};
	virtual std::string format() const;
	std::string execute(int &start);
private:
	NumericExpression* print;
};
class LetInt : public Command {
public:
	LetInt(int lineNumber, VariableInt* var, NumericExpression* exp, 
		std::map<std::string, VariableInt*> &imap) : Command(lineNumber),
	var(var), exp(exp), imap(&imap) {};
	virtual ~LetInt() {};
	virtual std::string format() const;
	virtual std::string execute(int &start);
private:
	VariableInt* var;
	NumericExpression* exp;
	std::map<std::string, VariableInt*>* imap;
};

class LetArray : public Command {
public:
	LetArray(int lineNumber, VariableArray* var, NumericExpression* exp, 
		std::multimap<std::string, VariableArray*> &amap) : Command(lineNumber),
	var(var), exp(exp), amap(&amap) {};
	virtual ~LetArray() {};
	virtual std::string format() const;
	virtual std::string execute(int &start);
private:
	VariableArray* var;
	NumericExpression* exp;
	std::multimap<std::string, VariableArray*>* amap;
};

class Go : public Command {
public:
	Go(int lineNumber, ConstantExpression* jline, 
	std::vector<Command*>* command_list): Command(lineNumber), jline(jline), 
	command_list(command_list) {};
	virtual ~Go() {};
	virtual std::string format() const = 0;
	virtual std::string execute(int &start) = 0;
protected:
	ConstantExpression* jline;
	std::vector<Command*>* command_list;
};
class GoSub : public Go {
public:
	GoSub(int lineNumber, ConstantExpression* jline, 
	std::vector<Command*>* command_list, std::stack<int>* substack) : 
	Go(lineNumber, jline, command_list), substack(substack) {};
	~GoSub() {};
	std::string format() const;
	std::string execute(int &start);
private:
	std::stack<int>* substack;
};
class GoTo : public Go {
public:
	GoTo(int lineNumber, ConstantExpression* jline, 
	std::vector<Command*>* command_list) : Go(lineNumber, jline, command_list) {};
	~GoTo() {};
	std::string format() const;
	std::string execute(int &start);
};
class If : public Go {
public:
	If(int lineNumber, ConstantExpression* jline, std::vector<Command*>* command_list,
	BinaryExpression* exp) : Go(lineNumber, jline, command_list), exp(exp) {};
	~If() {};
	std::string format() const;
	std::string execute(int &start);
private:
	BinaryExpression* exp;
};
class Return : public Command {
public:
	Return(int lineNumber, std::stack<int>* substack, 
		std::vector<Command*>* command_list) : Command(lineNumber), substack(substack),
		command_list(command_list) {};
	~Return() {};
	std::string format() const;
	std::string execute(int &start);
private:
	std::stack<int>* substack;
	std::vector<Command*>* command_list;
};
class End : public Command {
public:
	End(int lineNumber) : Command(lineNumber) {};
	~End() {};
	std::string format() const;
	std::string execute(int &start);
};
#endif