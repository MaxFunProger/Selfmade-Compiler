#pragma once
#include <map>
#include <set>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>
#include "ErrorSemant.h"

class ExpChecker {
public:
	ExpChecker();

	std::string check_op(std::string, std::string, std::string);
	std::string check_op(std::string, std::string);
private:
	std::map<std::string, std::map<std::string, std::map<std::string, std::string > > > bin_ops_;
	std::map<std::string, std::map<std::string, std::string> > uno_ops_;
	std::vector<std::string> types_;
};
