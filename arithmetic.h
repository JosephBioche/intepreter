#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#include <string>
#include <iostream>
#include <sstream>

class NumericExpression {
public:
    virtual ~NumericExpression() {};
    virtual std::string format() = 0;
    virtual int getValue () = 0;
    virtual std::string zero_error() const = 0;
    virtual std::string getType();
    bool isAlpha;
    std::string type;
};
class StringExpression : public NumericExpression {
public:
	StringExpression(std::string content);
	~StringExpression() {};
	std::string content;
	std::string format();
	int getValue();
 	std::string zero_error() const;
};
class ConstantExpression : public NumericExpression {
public:
	ConstantExpression(int value);
	virtual ~ConstantExpression() {};
	virtual std::string format();
    virtual std::string zero_error() const;
	int getValue();
	int value; 
};
class LineNumber : public ConstantExpression {
public:
    LineNumber(int value);
    virtual std::string zero_error() const;
    std::string format();
};
class Variable : public NumericExpression {
public:
	Variable(std::string name);
	virtual ~Variable () {};
	virtual std::string format() = 0;
	virtual int getValue ();
	virtual void setValue(int val);
    virtual std::string zero_error() const = 0;
protected:
	std::string name;
	int value;
	bool set;
};
class VariableInt : public Variable {
public:
	VariableInt(std::string name);
	~VariableInt() {};
	virtual std::string format();
    virtual std::string zero_error() const;
};
class VariableArray : public Variable {
public:
	VariableArray(std::string name, NumericExpression* i);
	~VariableArray() {};
	virtual std::string format();
    virtual std::string zero_error() const;
    int get_index();
    std::string getName();
    std::string mapName();
    NumericExpression* getIndex();
protected:
	NumericExpression* i;
};
class BinaryExpression : public NumericExpression {
public:
    BinaryExpression(NumericExpression* left, NumericExpression* right, std::string operand);
    virtual ~BinaryExpression() {};
	virtual std::string format();
	virtual int getValue() = 0;
    virtual std::string zero_error() const = 0;
protected:
    NumericExpression* left;
    NumericExpression* right;
    std::string operand;
};
class BoolExpression : public BinaryExpression {
public:
	BoolExpression(NumericExpression* left, NumericExpression* right, std::string operand);
	~BoolExpression() {};
	std::string format();
	virtual int getValue();
    virtual std::string zero_error() const;
};
class ArithmeticExpression : public BinaryExpression {
public:
	ArithmeticExpression(NumericExpression* left, NumericExpression* right, std::string operand);
	~ArithmeticExpression() {};
	virtual int getValue();
    std::string zero_error() const;
};
#endif
