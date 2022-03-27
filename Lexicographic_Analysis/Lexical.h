#pragma once
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>
#include <map>
#include <set>
#include <fstream>
#include "ErrorLex.h"

struct State {
	std::vector<State*> trans;
	bool is_terminal = false;
	std::string type;
};

struct Lex {
	Lex(int type, std::string val, int str_number) 
		: type(type)
		, val(val)
		, str_number(str_number) {}
	Lex() 
		: type(-1)
		, val("")
		, str_number(-1) {}
	int type;
	std::string val;

	int str_number;
};

class Lexical {
public:
	Lexical();
	Lex get_lex();
	void low();
	int get_index();
	void set_index(int);

	int index = -1;
	std::vector<Lex> lexems;

	std::vector<std::string> text;
	std::set<std::string> stuff;
	std::set<std::string> operators;
	std::set<std::string> separators;
	std::map<std::string, int> result;
	std::vector<State*> states;
	State* cur;
	State* prev;
	std::string text_queue = "";
};