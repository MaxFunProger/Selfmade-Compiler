#pragma once
#include "Syntactical.h"
#include "Lexical.h"



int main() {
	try {
		Lexical* lex = new Lexical();
		Syntactical* synt = new Syntactical(lex);
	}
	catch (ErrorLex err) {
		err.display();
	}



	return 0;
}