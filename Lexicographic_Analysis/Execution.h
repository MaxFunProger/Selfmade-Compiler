#pragma once
#include "Generation.h"


struct TID {
	TID() = default;
	TID(TID* parent) 
		: next(nullptr)
		, prev(parent) 
	{

	}

	TID* next;
	TID* prev;

	std::map<std::string, bool> is_visible_;
	std::map<std::string, std::string> var_type_; // get type by name

	std::map<std::string, int> var_int_; // name -> int value
	std::map<std::string, bool> var_bool_; // name -> bool value
	std::map<std::string, char> var_char_; // name -> char value
	std::map<std::string, std::string> var_string_; // name -> string value
	std::map<std::string, double> var_float_; // name -> float value

	int get_int(std::string);
	bool get_bool(std::string);
	char get_char(std::string);
	std::string get_string(std::string);
	double get_float(std::string);
};


class Executor {
public:
	Executor(std::vector<Atom> rpn) 
		: rpn_(rpn)
		, cnt_(0)
		, size_(rpn.size())
		, all_types_({ "int", "bool", "char", "string", "float", "void" })
	{
		Exec();
	}

	void Exec();
	void Add_TID();
	void Del_TID();
	void Up_TID();
	void Down_TID();
	TID* Find_Var_TID(std::string);
	TID* Find_Func_TID(std::string);
	

private:
	std::vector<Atom> rpn_;
	int cnt_;
	int size_;
	std::vector<std::string> all_types_;
	TID* CTID_, *FTID_;


};