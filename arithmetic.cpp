#include "arithmetic.h"
using namespace std;

//-----------------CONSTRUCTORS-----------------//
StringExpression::StringExpression(string content) {
	this->content = content;
	isAlpha = true;
	type = "stringExpression";
} 
ConstantExpression::ConstantExpression(int value){
	this->value = value;
	isAlpha = false;
	type = "constantExpression";
}
LineNumber::LineNumber(int value): ConstantExpression(value) {
	type = "lineNumber";
}
Variable::Variable(string name) {
	this->name = name;
	set = false;
	isAlpha = false;
	type = "variable";
}
VariableInt::VariableInt(string name): Variable(name) {
	type = "variableInt";
}
VariableArray::VariableArray(string name, NumericExpression* i): Variable(name) {
	this->i = i;
	type = "variableArray";
}
BinaryExpression::BinaryExpression(NumericExpression* left, NumericExpression* right, string operand) {
	this->left = left;
	this->right =right; 
	this->operand = operand;
	isAlpha = false;
	type = "binaryExpression";
}
BoolExpression::BoolExpression(NumericExpression* left, NumericExpression* right, 
	string operand): BinaryExpression(left, right, operand) {
	type = "boolExpression";
}
ArithmeticExpression::ArithmeticExpression(NumericExpression* left, 
	NumericExpression* right, string operand): BinaryExpression(left, right, operand) {
	type = "arithmeticExpression";
}
//--------------------FORMAT--------------------//
string ConstantExpression::format() {
	return to_string(value);
}
string StringExpression::format() {
	return content;
}
string LineNumber::format() {
	return "<" + to_string(value) + ">";
}
string VariableInt::format() {
	return name;
}
string VariableArray::format() {
	return name + "[" + i->format() + "]";
}
string BinaryExpression::format() {
    return "(" + this->left->format() + " " + operand + " " + 
    	this->right->format() + ")";
}
string BoolExpression::format() {
	return this->left->format() + " " + operand + " " + 
   	this->right->format();
}
//------------------ERROR CHECK-----------------//
string StringExpression::zero_error() const {
	return "none";
}
string ConstantExpression::zero_error() const {
	return "none";
}
string LineNumber::zero_error() const {
	return "none";
}
string VariableInt::zero_error() const {
	return "none";
}
string VariableArray::zero_error() const {
	return i->zero_error();
}
string BoolExpression::zero_error() const {
	return "none";
}
string ArithmeticExpression::zero_error() const {
	string message;
	message = right->zero_error();
	if (message != "none") {
		return message;
	}
	message = left->zero_error();
	if (message != "none") {
		return message;
	}
	if(right->getValue() == 0 && operand == "/") {
		message = ": Division by 0: " + left->format() + " = "
				+ to_string(left->getValue()) + ", " + right->format() + 
				" = " + to_string(right->getValue());
		return message;
	}
	return "none";
}
//-------------------GETVALUE-------------------//
int StringExpression::getValue() {
	return 0;
}
int ConstantExpression::getValue() {
	return value;
}
int Variable::getValue() {
	if(!set) {
		value = 0;
		set = true;
	}
	return value;
}
int BoolExpression::getValue() {
	if (operand == "=") {
		if(left->getValue() == right->getValue()) {
			return 1;
		}
		return 0;
	}
	if (operand == "<") {
		if(left->getValue() < right->getValue()) {
			return 1;
		}
		return 0;
	}
	if (operand == ">") {
		//cout << left->getValue() << " > " << right->getValue() << endl;
		if(left->getValue() > right->getValue()) {
			return 1;
		}
		return 0;
	}
	return 0;
}
int ArithmeticExpression::getValue() {
	if(operand == "+") {
		return left->getValue() + right->getValue();
	}
	if(operand == "-") {
		return left->getValue() - right->getValue();
	}
	if(operand == "*") {
		return left->getValue() * right->getValue();
	}
	if(operand == "/") {
		return left->getValue() / right->getValue();
	}
	return 0;
}
//-----------------MISCELLANEOUS-----------------//
string NumericExpression::getType() {
	return type;
}
void Variable::setValue(int val) {
	set = true;
	value = val;
}
int VariableArray::get_index() {
	return i->getValue();
}
string VariableArray::getName() {
	return name;
}
string VariableArray::mapName() {
	return name + to_string(i->getValue());
}
NumericExpression* VariableArray::getIndex() {
	return i;
}
