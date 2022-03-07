#pragma once
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <set>

struct Var {

	Var(std::string type, std::string name) : type(type), name(name) {}
	Var() : type(""), name("") {}


	std::string type;
	std::string name;
};

struct Func {

	Func(std::string type, std::string name, std::vector<std::pair<std::string, std::string> > params) : type(type), name(name), params(params) {}
	Func() : type(""), name("") {}

	std::string type;
	std::string name;
	std::vector<std::pair<std::string, std::string> > params;
};


class Local_TID {
public:

	Local_TID(Local_TID* parent) : parent_(parent), next_(nullptr) {}

	void push_func(Func, int);

	void push_var(Var);

	std::string find_var(std::string);

	std::string find_func(std::string);

	std::string find_var_all(std::string);

	std::string find_func_all(std::string);

	Local_TID* new_TID(Local_TID*);

	void del_TID(Local_TID*);

	std::map<std::string, Var> vars_;
	std::map<std::string, Func> funcs_;
	Local_TID* parent_;
	Local_TID* next_;

};

class TIDOperator {
public:
	void up();
	void down();
	void clear();

	std::string get_type_var(std::string);

	std::string get_type_func(std::string);
	Local_TID* cur;
};