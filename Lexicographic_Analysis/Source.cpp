#pragma once
#include "Syntactical.h"
#include "Lexical.h"
#include "Generation.h"
#include "Execution.h"



int main() {
	try {
		Lexical* lex = new Lexical();
		Syntactical* synt = new Syntactical(lex);
		lex->set_index(-1);
		Generation* gen = new Generation(lex);
		Executor* exec = new Executor();
	}
	catch (ErrorLex err) {
		err.display();
	}
	catch (...) {
		std::cout << "Unexpected situation\n";
	}



	return 0;
}