#pragma once
#include "Lexical.h"
#include <fstream>

struct Atom
{
	bool is_operator_;
	std::string val_;

	Atom(Lex l, bool is_operator) : is_operator_(is_operator), val_(l.val) { };

	Atom(std::string val, bool is_operator) : is_operator_(is_operator), val_(val) { };


};

struct Cycle
{
	int begin;
	std::vector <int> breaks;

	Cycle(int b) : begin(b) {}

};

class Generation
{
public:
	Generation(Lexical* lex) : lex_(lex) {
		Generator();
	}

	void Generator();
	void gen_fun();
	void gen_declaration();
	void gen_block(bool is_opened);
	void gen_expression();


	void gen_return();
	void gen_break();
	void gen_continue();
	void gen_cinout();
	void gen_if();
	void gen_while();
	void gen_for();
	void gen_dowhile();

	void print();


	void add_to_result(Atom a);

	Lexical* lex_;
	std::vector <Atom> result;
	std::vector <Cycle> cycle_stack;

	std::map <std::string, int> priority = { {"+", 6}, {"-", 6 }, {"=", 11},  {"*", 5}, {"/", 5}, {"!", 2}, {"%", 5},
		{"^", 7}, { ">", 8 }, { "<", 8 }, {"++", 1}, {"--", 1}, {"**", 3}, { "<=", 8 }, {"not", 4},  { ">=", 8 },
		{ "==", 8 }, { "+=", 8 },  { "-=", 11 }, { "*=", 11 }, { "/=", 11 }, { "|=", 11 }, { "%=", 11 }, { "^=", 11 },
		{ "&&", 9}, { "||", 10 }, { "|", 7 }, {"&", 7}, { "&=", 11 }, { "!=", 7 }, {"(", 12}, {"[", 12}, { "_", 4 }, {"~", 4} };

	std::set <std::string> right_assoc = { "**" , "+=","-=","*=","/=", "|=", "%=", "^=", "&=", "not" };

	std::map <std::string, int> func_table;
};

