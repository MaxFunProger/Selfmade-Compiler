#include "ErrorLex.h"

void ErrorLex::display() {
	std::cout << "(Lexical) Unexpected lexeme in line " << str_num_ << ": " << msg_ << "\n";
}