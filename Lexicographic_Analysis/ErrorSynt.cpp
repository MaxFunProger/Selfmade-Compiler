#include "ErrorSynt.h"

ErrorSynt::ErrorSynt(int str_num, std::string got, std::string expected)
	: str_num_(str_num)
	, got_(got)
	, expected_(expected) {
}


void ErrorSynt::display() {
	std::cout << "In line " << str_num_ << " got '" << got_ << "' but expected '" << expected_ << "'\n";
}
