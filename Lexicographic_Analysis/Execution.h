#pragma once
#include "Generation.h"
#include <math.h>


struct Function {
	Function() = default;
	Function(std::string type, std::string name, int params = 0) 
		: name(name)
		, type(type)
		, params_cnt(params)
	{
	
	}
	

	std::string type, name;
	std::vector<std::string> params;
	int params_cnt;
};

struct Rec {
	Rec() = default;
	Rec(int where, int tids = 0)
		: where(where)
		, tids(tids) 
	{

	}

	int where, tids; // where goto while return and how many tids need to delete
};

struct TID {
	TID() = default;
	TID(TID* parent) 
		: next(nullptr)
		, prev(parent) 
	{

	}

	TID* next;
	TID* prev;

	std::map<std::string, Function> funcs_;
	std::map<std::string, std::string> var_type_; // get type by name

	std::map<std::string, int> var_int_; // name -> int value
	std::map<std::string, bool> var_bool_; // name -> bool value
	std::map<std::string, char> var_char_; // name -> char value
	std::map<std::string, std::string> var_string_; // name -> string value
	std::map<std::string, double> var_float_; // name -> float value
};


class Executor {
public:
	Executor() 
		: cnt_(0)
		, CTID_(nullptr)
		, FTID_(nullptr)
		, LTID_(nullptr)
		, all_types_({ "int", "bool", "char", "string", "float", "void" })
	{
		CTID_ = nullptr;
		Exec();
	}

	void Exec();
	void Add_TID();
	void Del_TID();
	void Up_TID();
	void Down_TID();
	TID* Find_Var_TID(std::string); // returns the address of the variable (TID)
	std::string Get_Var_Type(std::string); // get type by name
	TID* Find_Func_TID(std::string);
	void Add_Var_Int(std::string, std::string, std::string = "0"); // type name value
	void Add_Var_Bool(std::string, std::string, std::string = "false"); // type name value
	void Add_Var_Char(std::string, std::string, std::string = " "); // type name value
	void Add_Var_String(std::string, std::string, std::string = ""); // type name value
	void Add_Var_Float(std::string, std::string, std::string = "0.0"); // type name value
	int Get_Var_Int(std::string);
	bool Get_Var_Bool(std::string);
	char Get_Var_Char(std::string);
	std::string Get_Var_String(std::string);
	double Get_Var_Float(std::string);
	void Add_Func(std::string, std::string);

	// rpn functions

	void Operate();
	

private:
	std::vector<Atom> rpn_;
	int cnt_;
	int size_; // total rpn size
	std::string val_;
	bool is_op_;
	std::vector<std::string> all_types_;
	TID* CTID_;
	TID* FTID_;
	TID* LTID_;
	std::vector<std::vector<Atom> > exstack_;
	std::vector<Rec> recstack_;
	std::ifstream file;
	std::ifstream in;
	std::ofstream out;
	std::string parse(std::string);
	void set_ops_uno();
	void set_ops_bin();
	void get_val(std::string&);
	int factorial(int);
	std::string op1_, op2_;
	int p_cnt_; // how many params func has
	std::vector<std::string> params_;
	std::string func_type_;	// type of the last func

};
