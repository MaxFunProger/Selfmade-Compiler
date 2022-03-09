#pragma once
#include <string>
#include <iostream>

class ErrorSemant {
public:
	ErrorSemant(std::string text) : str_num_(-1), text_(text) {}
	ErrorSemant(int str_num, std::string text) : str_num_(str_num), text_(text) {}
	void display();
	void set_line_number(int);
	int get_line_number();
private:
	int str_num_;
	std::string text_;
};