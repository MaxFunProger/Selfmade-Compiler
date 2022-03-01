#pragma once

#include <string>
#include <iostream>

class ErrorSynt {
public:
	ErrorSynt(int, std::string, std::string);
	void display();

private:
	int str_num_;
	std::string got_, expected_;
};