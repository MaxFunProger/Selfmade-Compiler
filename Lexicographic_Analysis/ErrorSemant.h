#pragma once
#include <string>
#include <iostream>

class ErrorSemant {
public:
	ErrorSemant(int str_num, std::string text) : str_num_(str_num), text_(text) {}
	void display();
private:
	int str_num_;
	std::string text_;
};