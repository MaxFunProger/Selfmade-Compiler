#pragma once
#include <map>
#include <vector>
#include <string>
#include <string.h>
#include <set>
#include "ErrorSemant.h"

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
	std::vector<std::pair<std::string, std::string> > params; // type : name
};


class Local_TID {
public:

	Local_TID(Local_TID* parent) : parent_(parent), next_(nullptr), return_(0), stopper_(0), func_type_("none") {
		if (parent_ != nullptr) {
			if (parent_->return_ == 1) {
				return_ = 1;
				func_type_ = parent_->func_type_;
			}
			if (parent_->stopper_ == 1) {
				stopper_ = 1;
			}
		}
	}

	void push_func(Func, int);

	void push_var(Var);

	std::string find_var(std::string);

	std::string find_func(std::string);

	std::string find_var_all(std::string);

	std::string find_func_all(std::string);

	std::vector<std::pair<std::string, std::string> > get_func_params(std::string);

	Local_TID* new_TID(Local_TID*);

	void del_TID(Local_TID*);

	std::map<std::string, Var> vars_;
	std::map<std::string, Func> funcs_;
	Local_TID* parent_;
	Local_TID* next_;
	bool return_;
	bool stopper_;
	std::string func_type_;
};

class TIDOperator {
public:
	void up();
	void down();
	void clear();

	std::string get_type_var(std::string);

	std::string get_type_func(std::string); // do not use because it finds just using name (no overload)
	Local_TID* cur;
};