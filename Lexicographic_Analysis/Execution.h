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
	TID* Find_Var_TID(std::string); // returns the address of the variable (TID)
	std::string Get_Var_Type(std::string); // get type by name
	TID* Find_Func_TID(std::string);
	void Add_Var_Int(std::string, std::string, int = 0); // type name value
	void Add_Var_Bool(std::string, std::string, bool = false); // type name value
	void Add_Var_Char(std::string, std::string, char = NULL); // type name value
	void Add_Var_String(std::string, std::string, std::string = ""); // type name value
	void Add_Var_Float(std::string, std::string, double = 0.0); // type name value
	int Get_Var_Int(std::string);
	bool Get_Var_Bool(std::string);
	char Get_Var_Char(std::string);
	std::string Get_Var_String(std::string);
	double Get_Var_Float(std::string);
	void Add_Func();

	// rpn functions

	void Operate();
	

private:
	std::vector<Atom> rpn_;
	int cnt_;
	int size_; // total rpn size
	std::string val_;
	bool is_op_;
	std::vector<std::string> all_types_;
	TID* CTID_, *FTID_, *LTID;
	std::vector<std::string> exstack_;
	std::vector<int> recstack_;

};