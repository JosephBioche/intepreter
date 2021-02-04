#include "command.h"
#include "arithmetic.h"
using namespace std;

string Print::format() const {
	return to_string(lineNumber) + " PRINT " + print->format();
}
string LetInt::format() const {
	return to_string(lineNumber) + " LET " + var->format() + " " + exp->format();
}
string LetArray::format() const {
	return to_string(lineNumber) + " LET " + var->format() + " " + exp->format();
}
string GoTo::format() const {
	return to_string(lineNumber) + " GOTO <" + jline->format() + ">";
}
string GoSub::format() const {
	return to_string(lineNumber) + " GOSUB <" + jline->format() + ">";
}
string If::format() const {
	return to_string(lineNumber) + " IF " + "[" + exp->format() + "] THEN <" + 
	jline->format() + ">";
}
string Return::format() const {
	return to_string(lineNumber) + " RETURN";
}
string End::format() const {
	return to_string(lineNumber) + " END";
}
string Print::execute(int &start) {
	string e = print->zero_error();
	if(e != "none") {
		return e;
	}
	else if (print->isAlpha) {
		cout << print->format() << endl;
		return "no error";
	}
	else {
		cout << print->getValue() << endl;
 		return "no error";
	}
}
string LetInt::execute(int &start) {
	string e = exp->zero_error();
	if(e != "none") {
		return e;
	}
	var->setValue(exp->getValue());
	return "no error";
}
//----------------------------------------------------------------------------//
string LetArray::execute(int &start) {
	string e = exp->zero_error();
	if(e != "none") {
		return e;
	}
	e = var->zero_error();
	if(e != "none") {
		return e;
	}
	var->setValue(exp->getValue());
	//if name already in map, reset in map
	typedef multimap<string, VariableArray*>::iterator iter;
	iter itr = amap->find(var->mapName());
	if (itr == amap->end()) { // doesn't exist yet
		pair<string, VariableArray*> newvar;
		newvar.first = var->mapName();
		newvar.second = var;
		amap->insert(newvar);
	}
	else {
		pair<iter,iter> range = amap->equal_range(var->mapName());

		iter itr = range.first;
		for(; itr != range.second; ++itr) { // if same name + and index, update value
			if(var->zero_error() == "none" && (itr->second)->zero_error() == "none") {
				if((itr->second)->get_index() == var->get_index()) {
					(itr->second)->setValue(exp->getValue());
				}
			}
		}
	}
 	return "no error";
}
string GoTo::execute(int &start) {
	int y = 0;
	int x = cfind(jline->getValue(), command_list, y, command_list->size()-1); // Return index												   
	if(x == -1) {
		return ": GOTO to non-existent line "+ to_string(jline->getValue());
	}
	start = x - 1;
 	return "no error";
}
string GoSub::execute(int &start) {
	int y = 0;
	int x = cfind(jline->getValue(), command_list, y, command_list->size()-1); // Return index												   
	
	if(x == -1) {
		return ": GOSUB to non-existent line "+ to_string(jline->getValue());
	}
	start = x - 1;
	substack->push(lineNumber);
 	return "no error";
}
string If::execute(int &start) {
	int y = 0;
	int x = cfind(jline->getValue(), command_list, y, command_list->size()-1); // Return index -1 to account for next increment
	if(x == -1) {
			return ": IF jump to non-existent line "+ to_string(jline->getValue());
		}
	if (exp->getValue()) {
		start = x - 1;
	}
	return "no error";
}
string Return::execute(int &start) {
	if (substack->empty()) {
		return ": No matching GOSUB for RETURN";
	}
	start = cfind(substack->top(), command_list, 0, command_list->size()-1);
	string nextCommand = command_list->at(start + 1)->format();
	substack->pop();
	return "no error";
 	
}
string End::execute(int &start) {
 	return "end";
}
int Command::cfind(int j, vector<Command*>* list, int start, int end) { // returns iterator
	if(list->at(end)->lineNumber == j) {
		return end;
	}
	if(list->at(start)->lineNumber == j) {
		return start;
	}
	if((list->at((end-start)/2)->lineNumber) == j) {
		return (end-start)/2;
	}
	if((list->at((end-start)/2)->lineNumber) > j) {
		return cfind(j, list, start+1, (end-start)/2 -1);
	}
	if((list->at((end-start)/2)->lineNumber) < j) {
		return cfind(j, list, (end-start)/2 +1, end-1);
	}
	return -1;
}
