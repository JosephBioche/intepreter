#include "interpreter.h"
#include "command.h"
#include "arithmetic.h"
#include <cstring>
#include <cctype>
#include <iterator>

using namespace std;

Interpreter::Interpreter(istream& in, string fileName) {
	error = "none";
	readError = false;
	this->fileName = fileName;
	command_list = new vector<Command*>;
	memcheck = new vector<NumericExpression*>;
	cout << "Starting " << fileName << "..."<< endl;
    this->parse(in);
    if(error != "none") {
    	cout << "Error: " + error << endl;
    }
}
Interpreter::~Interpreter() {
	for(int i = 0; i < (int) command_list->size(); i++) {
		if(command_list->at(i) != NULL) {
			Command* temp = command_list->at(i);
			command_list->at(i) = NULL;
			delete temp;
		}
	}
	for(int i = 0; i < (int) memcheck->size(); i++) {
		if(memcheck->at(i) != NULL) {
			NumericExpression* temp = memcheck->at(i);
			memcheck->at(i) = NULL;
			delete temp;
		}
	}
	delete command_list;
	delete memcheck;
} 
bool Interpreter::read_error() {
	return readError;
}
void Interpreter::parse(istream& in) {
    string line = "null";
    int count = 0;
    while (getline(in, line) && (error == "none")) {
    	count++;
    	string command = "";
   		int line_number;
    	string cleanline;

        stringstream stream(line);
        stream >> line_number; // get line #
        stream >> command; // get command name
        getline(stream, cleanline);
        int x = 0;
        Command* a = NULL;
        if(command == "PRINT") { // get PRINT
			NumericExpression* print = ParseNumericExpression(cleanline, x);
			if(print == NULL) {
				cout << "Error in Line " + to_string(line_number) + ": " + 
					error << endl;
				readError = true;
				return;
			}
			a = new Print(line_number, print);	
		}
		else if(command == "LET") { // get LET
			string name = ParseVariableName(cleanline, x);
			if(cleanline[x] == '[') {
				NumericExpression* index = ParseNumericExpression(cleanline, ++x);
				ClearWhiteSpace(cleanline, x);
				x++; // eat the ']'
				NumericExpression* exp = ParseNumericExpression(cleanline, x);
				if((index == NULL) || (exp == NULL)) {
					break;
				}
				string mapName = name + to_string(index->getValue());
				VariableArray* var = VariableArrayExists(mapName);
				if(var == NULL) {
					var = new VariableArray(name, index); // track variable
					pair<string, VariableArray*> newvar;
					newvar.first = mapName;
					newvar.second = var;
					amap.insert(newvar);
					memcheck->push_back(var);
				}
				
				a = new LetArray(line_number, var, exp, amap);
			}
			else {
				VariableInt* var = VariableIntExists(name);
				if(var == NULL) {
					var = new VariableInt(name);
					memcheck->push_back(var);
					pair<string, VariableInt*> newvar;
					newvar.first = name;
					newvar.second = var;
					imap.insert(newvar);
				}
				NumericExpression* exp = ParseNumericExpression(cleanline, x);
				if(exp == NULL) {
					break;
				}
				a = new LetInt(line_number, var, exp, imap);
			} 
			ClearWhiteSpace(cleanline, x);
		}
		else if(command == "GOTO") { // get GOTO
			ConstantExpression* jline = ParseConstant(cleanline, x);
			a = new GoTo(line_number, jline, command_list);
		}
		else if(command == "GOSUB") { // get GOSUB
			ConstantExpression* jline = ParseConstant(cleanline, x);
			a = new GoSub(line_number, jline, command_list, &substack);
		}
		else if(command == "IF") { // get IF
			BinaryExpression* exp = ParseBinaryExpression(cleanline, x);
			if(exp == NULL) {
				break;
			}
			ClearWhiteSpace(cleanline, x);
			ConstantExpression* jline = ParseConstant(cleanline, x);
			a = new If(line_number, jline, command_list, exp);
		}
		else if(command == "RETURN") { // get RETURN
			a = new Return(line_number, &substack, command_list);
		}
		else if(command == "END") { // get END
			a = new End(line_number);
		}
		command_list->push_back(a);
    }
    cout << "Parsed " << command_list->size() << " commands:" << endl;
    for(int i = 0; i < (int) command_list->size(); i++) {
    	cout << "	" << command_list->at(i)->format() << endl;
    }
}
NumericExpression* Interpreter::ParseNumericExpression(string line, int& start) {
	ClearWhiteSpace(line, start);
	if(line[start] == '('){ // Binary Expression
		start++;
		return ParseBinaryExpression(line, start);
	}
	if(isalpha(line[start])) { // Variable
		return ParseVariable(line, start);
	}
	if(isdigit(line[start])) { // Constant
		return ParseConstant(line, start);
	}
	if(line[start] == '\"') {
		return ParseString(line, ++start);
	}
	else if(line[start] == '-' ) { // negative
		if(isalpha(line[start + 1])) {
			return ParseVariable(line, start);
		}
		if(isdigit(line[start + 1])) {
			return ParseConstant(line, start);
		}
	}
	else if(line[start] == '<') { // jline
		return ParseConstant(line, start);
	}
	return NULL;
}
BinaryExpression* Interpreter::ParseBinaryExpression(string line, int& start) {
	NumericExpression* left = ParseNumericExpression(line, start); // get left
	ClearWhiteSpace(line, start);
	string operand = line.substr(start++, 1); // Get operand
	NumericExpression* right = ParseNumericExpression(line, start); // get right
	ClearWhiteSpace(line, start);

	bool typeErr = (left->getType() == "stringExpression") || (right->getType() == "stringExpression");
	if(typeErr) {
		error = " Cannot have string in binary expression";
		return NULL;
	}

	if(line[start] == ')') { // If arithmeticExpression
		++start;
		ArithmeticExpression* a = new ArithmeticExpression(left, right, operand);
		memcheck->push_back(a);
		return a;
	}
	if(line.substr(start, 4) == "THEN") { // If booleanExpression
		start += 4;
		BoolExpression* a = new BoolExpression(left, right, operand);
		memcheck->push_back(a);
		return a;
	}
	return NULL;
}
ConstantExpression* Interpreter::ParseConstant(string line, int& start) {
	ClearWhiteSpace(line, start);
	string integer = "";
	if (line[start] == '<') { // jline
		++start;
	}
	if (line[start] == '-') { // negative
		integer += '-';
		++start;
	}
	ClearWhiteSpace(line, start);
	while(isdigit(line[start])) {
		integer += line[start++];
	}
	
	ConstantExpression* a = new ConstantExpression(stoi(integer));
	memcheck->push_back(a);
	return a;
}
StringExpression* Interpreter::ParseString(string line, int& start) {
	string word = "";
	while(line[start] != '\"') {
		word += line[start++];
	}
	start++; // eat the final "

	StringExpression* wordStrEx = new StringExpression(word);
	memcheck->push_back(wordStrEx);
	return wordStrEx;
}
Variable* Interpreter::ParseVariable(string line, int& start) {
	ClearWhiteSpace(line, start);
	string name = ParseVariableName(line, start); // get name of variable
	ClearWhiteSpace(line, start);
	if(line[start] == '[') { // for ARRAYS
		NumericExpression* index = ParseNumericExpression(line, ++start);
		if(index == NULL) {
			return NULL;
		}
		ClearWhiteSpace(line, start);
		++start; // Eat the "]"
		string mapName = name + to_string(index->getValue());
		VariableArray* a = VariableArrayExists(mapName);
		if(a == NULL) {
			a = new VariableArray(name, index); // track variable
			pair<string, VariableArray*> newvar;
			newvar.first = mapName;
			newvar.second = a;
			amap.insert(newvar);
			memcheck->push_back(a);
		}
		return a;
	}
	else { // for SIMPLE VARIABLES
		VariableInt* a = VariableIntExists(name);
		if(a == NULL) {
			a = new VariableInt(name);
			memcheck->push_back(a);

			pair<string, VariableInt*> newvar;
			newvar.first = name;
			newvar.second = a;
			imap.insert(newvar);
		 }
		return a;
	}
}
string Interpreter::ParseVariableName(string line, int& start) {
	ClearWhiteSpace(line, start);
	string name = "";
	if (line[start] == '-') {
		name += '-';
		start++;
	}
	ClearWhiteSpace(line, start);
	while(isalpha(line[start])) {
		name += line[start++];
	}
	ClearWhiteSpace(line, start);
	return name;
}
void Interpreter::write(ostream& out) {
	if (!command_list->empty()) {
		for(int i = 0; i < (int)command_list->size(); i++) {
			out << (command_list->at(i))->format() << endl;
		}
	}
}
void Interpreter::execute() {
	if ((command_list->empty()) || (error != "none")) {
		cout << "Quitting..." << endl;
		return;
	}
	cout << "executing..." << endl;
	for(int i = 0; i < (int) command_list->size(); i++) {
		string errorMessage = (command_list->at(i))->execute(i);
		if(errorMessage == "end") {
			return;
		}
		if(errorMessage != "no error") { // Error check
			cout << "Error in Line " << command_list->at(i)->lineNumber
			<< errorMessage << "." << endl;
			return;
		}
	} 
	cout << "Finished running " << fileName << "!" << endl;
}
void Interpreter::ClearWhiteSpace(string line, int& start) {
	while(isspace(line[start])) {
		start++;
	}
}
string Interpreter::ClearWhiteSpaceString(string str) {
	int i = 0;
	string retStr = "";
	while(i < (int) str.size()) {
		if((str[i] != ' ') && (str[i] != '\n')) {
			retStr += str[i];
		}
		i++;
	}
	return retStr;
}
bool Interpreter::zero_error(NumericExpression* x) {
	string a = x->zero_error();
	if (a == "none") {
		return false;
	}
	else {
		error = a;
		return true;
	}
}
VariableInt* Interpreter::VariableIntExists(string name) {
	map<string, VariableInt*>::iterator i = imap.find(name);
	if (i == imap.end()) { // If it doesn't exist
		return NULL;
	}
	return i->second; // If it does exist, return pointer
}
VariableArray* Interpreter::VariableArrayExists(string name) {
	multimap<string, VariableArray*>::iterator i = amap.find(name);
	if (i == amap.end()) { // If it doesn't exist
		return NULL;
	}
	return i->second; // If it does exist, return pointer
}