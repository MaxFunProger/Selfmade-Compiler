#pragma once
#include "Lexical.h"
#include "ErrorSynt.h"
#include "TID.h"
#include "Semantical.h"
#include "ErrorSemant.h"


struct Type {
	Type() : type(""), index(0), call(0) {};
	Type(std::string t, int ind, int call) : type(t), index(ind), call(call) {};
	std::string type;
	int index, call;
};

class Syntactical {
public:
	Syntactical(Lexical*);

	void Program();
	void Value();
	void Variable();
	void Stopper();
	void Block();
	void Operator();
	void DerivedOperator();
	void InOut();
	void ListInOut();
	void FuncDeclaration();
	void DecFuncParameters();
	void Declaration();
	void Section();
	void LoopOperator();
	void ConditionalOperator();
	void While();
	void DoWhile();
	void For();
	void If();
	void Parameters(std::vector<std::pair<std::string, std::string> >&);
	void FuncCall();
	void Expression();
	std::string ReturnExpression();
	bool Priority1(std::string);
	bool Priority2(std::string);
	bool Priority3(std::string);
	bool Priority4(std::string);
	bool Priority5(std::string);
	bool Priority6(std::string);
	bool Priority7(std::string);
	bool Priority8(std::string);
	bool Priority9(std::string);
	bool Priority10(std::string);
	bool PriorityNot(std::string);
	void Atom1();
	void Atom2();
	void Atom3();
	void Atom4();
	void Atom5();
	void Atom6();
	void Atom7();
	void Atom8();
	void Atom9();
	void Atom10();
	void AtomNot();
	std::string Array(std::string = "none");
	void clear();
	void check_op(int);
	void push(std::string, int = 0, int = 0);
	void stack_clear();
	Type pop();
	int check_ind();
	bool is_castable(std::string, std::string);
	int array_check(int);

private:
	Lexical* lex_;
	Lex lexem_;
	void gl();
	void low(int);
	void set_val(std::string);
	int& type_;
	std::string& val_;
	TIDOperator* oper_;
	ExpChecker* checker_;
	std::string nameo_, typeo_, typev_, namev_;
	std::vector<std::pair<std::string, std::string> > params_;
	std::vector<Type> stack_;
	std::set<std::string> ops_;
	std::map<std::string, std::map<std::string, int> > cast_table_;
	std::string dec_type_;
	int max_depth_;
	bool decl_;
	int index_;
	int call_;
	int was_stopper_;
	std::vector<std::string> index_vec_;
	std::string array_type_;

};