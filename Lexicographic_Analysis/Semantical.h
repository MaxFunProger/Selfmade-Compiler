#pragma once
#include <map>
#include <set>
#include <string>
#include <string.h>
#include <vector>
#include <fstream>

class ExpChecker {
public:
	ExpChecker();

	void check_op(std::string, std::string, std::string);
	void check_op(std::string, std::string);
private:
	std::map<std::string, std::map<std::string, std::map<std::string, std::string > > > bin_ops_;
	std::map<std::string, std::map<std::string, std::string> > uno_ops_;
	std::vector<std::string> types_;
};
