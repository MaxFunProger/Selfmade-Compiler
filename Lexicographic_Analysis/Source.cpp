#pragma once
#include "Syntactical.h"



int main() {
	Lexical* lex = new Lexical();
	Syntactical* synt = new Syntactical(lex);



	return 0;
}