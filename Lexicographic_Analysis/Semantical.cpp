#include "Semantical.h"

ExpChecker::ExpChecker() {
	int n;
	types_ = { "int", "bool", "char", "string", "float" };
	std::ifstream file("opers_types.txt");
	file >> n;
	std::string sign, op1, op2 = "", ret;
	std::vector<std::string> types;
	int flag;
	for (int i = 0; i < n - 3; ++i) {
		file >> sign;
		for (auto& j : types_) {
			for (auto& g : types_) {
				bin_ops_[sign][j][g] = "none";
			}
		}
		for (int g = 0; g < 5; ++g) {
			file >> op1;
			while (op2 != ":") {
				file >> op2;
				types.push_back(op2);
			}
			file >> ret;
			for (auto& j : types) {
				if (j == "float") {
					bin_ops_[sign][op1][j] = "float";
				}
				else {
					bin_ops_[sign][op1][j] = ret;
				}
			}
			if (g < 4) {
				file >> sign;
			}
		}
	}
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 5; ++j) {
			file >> sign >> op1 >> flag;
			if (flag) {
				file >> ret;
				uno_ops_[sign][op1] = ret;
			}
			else {
				uno_ops_[sign][op1] = "none";
			}
		}
	}
	
}

void ExpChecker::check_op(std::string sign, std::string op1, std::string op2) {
	if (bin_ops_[sign][op1][op2] == "none") {
		throw; // unexpected operands
	}
}

void ExpChecker::check_op(std::string sign, std::string op1) {
	if (uno_ops_[sign][op1] == "none") {
		throw; // unexpected operands
	}
}