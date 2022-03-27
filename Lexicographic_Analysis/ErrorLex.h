#pragma once
#include <string>
#include <iostream>

class ErrorLex {
public:
	ErrorLex(int str_num, std::string msg) : str_num_(str_num), msg_(msg) {}

	void display();

private:
	int str_num_;
	std::string msg_;
};