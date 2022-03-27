#include "ErrorSemant.h"

void ErrorSemant::display() {
	std::cout << "(Semantical) In line " << str_num_ << ' ' + text_ << "\n";
}

void ErrorSemant::set_line_number(int str_num) {
	str_num_ = str_num;
}

int ErrorSemant::get_line_number() {
	return str_num_;
}