#include "ErrorSemant.h"

void ErrorSemant::display() {
	std::cout << "In line " << str_num_ << ' ' + text_ << "\n";
}