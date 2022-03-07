#pragma once
#include "Lexical.h"
#include "ErrorSynt.h"
#include "TID.h"
#include "Semantical.h"
#include "ErrorSemant.h"

class Syntactical {
public:
	Syntactical(Lexical*);

	void Program();
	void Type();
	void Int();
	void Float();
	void Bool();
	void String();
	void Char();
	void Num();
	void Digit();
	void Letter();
	void Value();
	void Sign();
	void Name();
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
	void Parameters();
	void FuncCall();
	void Expression();
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
	void Array();
	void clear();
	void check_op(int);
	void push(std::string);
	std::string pop();
	int check_ind();

private:
	Lexical* lex_;
	Lex lexem_;
	void gl();
	void low(int);
	int& type_;
	std::string& val_;
	TIDOperator* oper_;
	ExpChecker* checker_;
	std::string nameo_, typeo_, typev_, namev_;
	std::vector<std::pair<std::string, std::string> > params_;
	std::vector<std::string> stack_;
	std::set<std::string> ops_;
};