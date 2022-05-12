#include "Execution.h"


std::string Executor::parse(std::string str) {
	str.pop_back();
	reverse(str.begin(), str.end());
	str.pop_back();
	reverse(str.begin(), str.end());
	return str;
}

void Executor::get_val(std::string& op1_) {
	if (Find_Var_TID(op1_) != nullptr) {
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			op1_ = Get_Var_String(op1_);
		}
		else if (type == "char") {
			op1_ = "'" + std::to_string(Get_Var_Char(op1_));
			op1_.push_back('\'');
		}
		else if (type == "int") {
			op1_ = std::to_string(Get_Var_Int(op1_));
		}
		else if (type == "bool") {
			if (Get_Var_Bool(op1_)) {
				op1_ = "1";
			}
			else {
				op1_ = "0";
			}
		}
		else if (type == "float") {
			op1_ = std::to_string(Get_Var_Float(op1_));
		}
	}
}

int Executor::factorial(int a) {
	if (a < 0) {
		throw; // incorrect factorial argument
	}
	if (a == 0) {
		return 1;
	}
	else {
		int res = 1;
		for (int i = 2; i <= a; ++i) {
			res *= i;
		}
		return res;
	}
}

void Executor::set_ops_bin() {
	op2_ = exstack_.back().val_;
	exstack_.pop_back();
	op1_ = exstack_.back().val_;
	exstack_.pop_back();
}

void Executor::set_ops_uno() {
	op1_ = exstack_.back().val_;
	exstack_.pop_back();
}

void Executor::Add_TID() {
	CTID_->next = new TID(CTID_);
	CTID_ = CTID_->next;
	FTID_ = CTID_;
}

void Executor::Del_TID() {
	FTID_ = CTID_ = CTID_->prev;
	delete CTID_->next;
}

void Executor::Up_TID() {
	FTID_ = FTID_->prev;
}

void Executor::Down_TID() {
	FTID_ = FTID_->next;
}

TID* Executor::Find_Var_TID(std::string name) {
	if (FTID_ == nullptr) {
		FTID_ = CTID_;
		LTID_ = nullptr;
		return nullptr;
	}
	while (FTID_ != nullptr && FTID_->var_type_.count(name) == 0) {
		Up_TID();
	}
	if (FTID_ != nullptr) {
		LTID_ = FTID_;
		FTID_ = CTID_;
		return LTID_;
	}
	else {
		FTID_ = CTID_;
		LTID_ = nullptr;
		return nullptr;
	}
}

void Executor::Add_Var_Int(std::string type, std::string name, std::string val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_int_[name] = std::stoi(val);
}

void Executor::Add_Var_Bool(std::string type, std::string name, std::string val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_bool_[name] = val == "false" || val == "0" ? false : true;
}

void Executor::Add_Var_Char(std::string type, std::string name, std::string val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_char_[name] = val[0];
}

void Executor::Add_Var_String(std::string type, std::string name, std::string val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_string_[name] = val;
}

void Executor::Add_Var_Float(std::string type, std::string name, std::string val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_float_[name] = std::stof(val);
}

int Executor::Get_Var_Int(std::string name)
{
	return LTID_->var_int_[name];
}

bool Executor::Get_Var_Bool(std::string name)
{
	return LTID_->var_bool_[name];
}

char Executor::Get_Var_Char(std::string name)
{
	return LTID_->var_char_[name];
}

std::string Executor::Get_Var_String(std::string name)
{
	return LTID_->var_string_[name];
}

double Executor::Get_Var_Float(std::string name)
{
	return LTID_->var_float_[name];
}

std::string Executor::Get_Var_Type(std::string name) {
	TID* tmp = Find_Var_TID(name);
	return tmp->var_type_[name];
}

/*TID* Executor::Find_Func_TID(std::string name) {
	if (FTID_ == nullptr) {
		return nullptr;
	}
	if (FTID_->is_visible_[name]) {
		return FTID_;
	}
	else {
		Up_TID();
	}
}*/


void Executor::Operate() {
	if (rpn_[cnt_].val_ == "create_fun") {
		//Add_Func(); // TODO
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "goto") {
		cnt_ = std::stoi(exstack_.back().val_);
		exstack_.pop_back();
	}
	else if (rpn_[cnt_].val_ == "if_not_goto") {
		int address = std::stoi(exstack_.back().val_);
		exstack_.pop_back();
		if (exstack_.back().val_ == "false") {
			cnt_ = address;
		}
		else {
			++cnt_;
		}
		exstack_.pop_back();
	}
	else if (rpn_[cnt_].val_ == "create_var") {
		std::string type = exstack_.back().val_;
		exstack_.pop_back();
		std::string name = exstack_.back().val_;
		exstack_.pop_back();
		if (exstack_.size() && !exstack_.back().is_operator_) {
			std::string value = exstack_.back().val_;
			exstack_.pop_back();
			if (type == "int") {
				Add_Var_Int(type, name, value);
			}
			else if (type == "bool") {
				Add_Var_Bool(type, name, value);
			}
			else if (type == "char") {
				Add_Var_Char(type, name, value);
			}
			else if (type == "string") {
				Add_Var_String(type, name, value);
			}
			else if (type == "float") {
				Add_Var_Float(type, name, value);
			}
		}
		else {
			if (type == "int") {
				Add_Var_Int(type, name);
			}
			else if (type == "bool") {
				Add_Var_Bool(type, name);
			}
			else if (type == "char") {
				Add_Var_Char(type, name);
			}
			else if (type == "string") {
				Add_Var_String(type, name);
			}
			else if (type == "float") {
				Add_Var_Float(type, name);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "create_tid") {
		Add_TID();
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "clear_oparand_stack") {
		exstack_.clear();
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "return") {
		// function returns smth
	}
	else if (rpn_[cnt_].val_ == "print") {
		std::string val = exstack_.back().val_;
		if (Find_Var_TID(val) != nullptr) {
			std::string type = Get_Var_Type(val);
			if (type == "int") {
				out << LTID_->var_int_[val];
			}
			else if (type == "char") {
				out << LTID_->var_char_[val];
			}
			else if (type == "string") {
				std::string val = LTID_->var_string_[val];
				if (val == "\n") {
					out << '\n';
				}
				else if (val == "\t") {
					out << '\t';
				}
				else {
					out << val;
				}
			}
			else if (type == "bool") {
				if (LTID_->var_bool_[val]) {
					out << true;
				}
				else {
					out << false;
				}
			}
			else if (type == "float") {
				out << LTID_->var_float_[val];
			}
			else {
				throw; // unexpected type
			}
		}
		else {
			std::string val = exstack_.back().val_;
			if (val == "\"\\n\"") {
				out << '\n';
			}
			else if (val == "\"\\t\"") {
				out << '\t';
			}
			else {
				out << val;
			}
		}
		exstack_.pop_back();
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "scan") {
		std::string var = exstack_.back().val_;
		exstack_.pop_back();
		Find_Var_TID(var);
		if (LTID_->var_type_[var] == "int") {
			std::cin >> LTID_->var_int_[var];
		}
		else if (LTID_->var_type_[var] == "bool") {
			std::cin >> LTID_->var_bool_[var];
		}
		else if (LTID_->var_type_[var] == "char") {
			std::cin >> LTID_->var_char_[var];
		}
		else if (LTID_->var_type_[var] == "string") {
			std::cin >> LTID_->var_string_[var];
		}
		else if (LTID_->var_type_[var] == "float") {
			std::cin >> LTID_->var_float_[var];
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "+") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] + parse(op2_)[0]), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] + std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) + std::stod(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) + std::stod(op2_)), 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) + std::stoi(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) + std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "+=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] += parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] += std::stoi(op2_);
			}
		}
		else if (type == "float") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_float_[op1_] += std::stod(parse(op2_));
			}
			else {
				FTID_->var_float_[op1_] += std::stod(op2_);
			}
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] += std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] += std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] += std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] += std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "-") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] - parse(op2_)[0]), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] - std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) - std::stod(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) - std::stod(op2_)), 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) - std::stoi(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) - std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "-=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] -= parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] -= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_float_[op1_] -= std::stod(parse(op2_));
			}
			else {
				FTID_->var_float_[op1_] -= std::stod(op2_);
			}
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] -= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] -= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] -= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] -= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "*") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] * parse(op2_)[0]), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] * std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) * std::stod(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) * std::stod(op2_)), 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) * std::stoi(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) * std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "*=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] *= parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] *= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_float_[op1_] *= std::stod(parse(op2_));
			}
			else {
				FTID_->var_float_[op1_] *= std::stod(op2_);
			}
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] *= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] *= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] *= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] *= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "/") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] / parse(op2_)[0]), 0));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] / std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				double op2 = std::stod(parse(op2_));
				if (op2 == 0.0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) / std::stod(parse(op2_))), 0));
			}
			else {
				double op2 = std::stod(op2_);
				if (op2 == 0.0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(std::stod(op1_) / std::stod(op2_)), 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) / std::stoi(parse(op2_))), 0));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) / std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "/=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_char_[op1_] /= parse(op2_)[0];
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_char_[op1_] /= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				double op2 = std::stod(parse(op2_));
				if (op2 == 0.0) {
					throw; // division by 0
				}
				FTID_->var_float_[op1_] /= std::stod(parse(op2_));
			}
			else {
				double op2 = std::stod(op2_);
				if (op2 == 0.0) {
					throw; // division by 0
				}
				FTID_->var_float_[op1_] /= std::stod(op2_);
			}
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_bool_[op1_] /= std::stoi(parse(op2_));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_bool_[op1_] /= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_int_[op1_] /= std::stoi(parse(op2_));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_int_[op1_] /= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "%") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] % parse(op2_)[0]), 0));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] % std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			;
		}
		else {
			if (op2_[0] == '\'') {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) % std::stoi(parse(op2_))), 0));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) % std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "%=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_char_[op1_] %= parse(op2_)[0];
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_char_[op1_] %= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			;
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_bool_[op1_] %= std::stoi(parse(op2_));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_bool_[op1_] %= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				int op2 = std::stoi(parse(op2_));
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_int_[op1_] %= std::stoi(parse(op2_));
			}
			else {
				int op2 = std::stoi(op2_);
				if (op2 == 0) {
					throw; // division by 0
				}
				FTID_->var_int_[op1_] %= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "^") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] ^ parse(op2_)[0]), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] ^ std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			;
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) ^ std::stoi(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) ^ std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "^=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] ^= parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] ^= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			;
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] ^= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] ^= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] ^= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] ^= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "&") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] & parse(op2_)[0]), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] & std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			;
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) & std::stoi(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) & std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "^=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] &= parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] &= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			;
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] &= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] &= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] &= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] &= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "|") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] | parse(op2_)[0]), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(parse(op1_)[0] | std::stoi(op2_)), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			;
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) | std::stoi(parse(op2_))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::stoi(op1_) | std::stoi(op2_)), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "|=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			//FTID_->var_string_[op1_] += parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] |= parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] |= std::stoi(op2_);
			}
		}
		else if (type == "float") {
			;
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] |= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] |= std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] |= std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] |= std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "**") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			//exstack_.push_back(Atom(parse(op1_) + parse(op2_), 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string((int)std::pow(parse(op1_)[0], std::stoi(parse(op2_)))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string((int)std::pow(parse(op1_)[0], std::stoi(op2_))), 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string(std::pow(parse(op1_)[0], std::stoi(parse(op2_)))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string(std::pow(parse(op1_)[0], std::stoi(op2_))), 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::to_string((int)std::pow(std::stoi(op1_), std::stoi(parse(op2_)))), 0));
			}
			else {
				exstack_.push_back(Atom(std::to_string((int)std::pow(std::stoi(op1_), std::stoi(op2_))), 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "=") {
		set_ops_bin();
		if (Find_Var_TID(op1_) == nullptr) {
			throw; // rvalue given but lvalue expected
		}
		get_val(op2_);
		std::string type = Get_Var_Type(op1_);
		if (type == "string") {
			FTID_->var_string_[op1_] = parse(op2_);
		}
		else if (type == "char") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_char_[op1_] = parse(op2_)[0];
			}
			else {
				FTID_->var_char_[op1_] = std::stoi(op2_);
			}
		}
		else if (type == "float") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_float_[op1_] = std::stod(parse(op2_));
			}
			else {
				FTID_->var_float_[op1_] = std::stod(op2_);
			}
		}
		else if (type == "bool") {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_bool_[op1_] = (bool)std::stoi(parse(op2_));
			}
			else {
				FTID_->var_bool_[op1_] = (bool)std::stoi(op2_);
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '\'') != op2_.end()) {
				FTID_->var_int_[op1_] = std::stoi(parse(op2_));
			}
			else {
				FTID_->var_int_[op1_] = std::stoi(op2_);
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == ">") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) > parse(op2_) == true ? "1" : "0", 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'' || op2_[0] == '\"') {
				exstack_.push_back(Atom(parse(op1_)[0] > parse(op2_)[0] == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(parse(op1_)[0] > std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stod(op1_) > std::stod(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) > std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) > std::stod(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stoi(op1_) > std::stoi(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) > std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) > std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}

	else if (rpn_[cnt_].val_ == ">=") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) >= parse(op2_) == true ? "1" : "0", 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'' || op2_[0] == '\"') {
				exstack_.push_back(Atom(parse(op1_)[0] >= parse(op2_)[0] == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(parse(op1_)[0] >= std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stod(op1_) >= std::stod(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) >= std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) >= std::stod(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stoi(op1_) >= std::stoi(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) >= std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) >= std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}

	else if (rpn_[cnt_].val_ == "<") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) < parse(op2_) == true ? "1" : "0", 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'' || op2_[0] == '\"') {
				exstack_.push_back(Atom(parse(op1_)[0] < parse(op2_)[0] == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(parse(op1_)[0] < std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stod(op1_) < std::stod(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) < std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) < std::stod(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stoi(op1_) < std::stoi(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) < std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) < std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}

	else if (rpn_[cnt_].val_ == "<=") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) <= parse(op2_) == true ? "1" : "0", 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'' || op2_[0] == '\"') {
				exstack_.push_back(Atom(parse(op1_)[0] <= parse(op2_)[0] == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(parse(op1_)[0] <= std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stod(op1_) <= std::stod(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) <= std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) <= std::stod(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stoi(op1_) <= std::stoi(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) <= std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) <= std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}

	else if (rpn_[cnt_].val_ == "==") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) == parse(op2_) == true ? "1" : "0", 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'' || op2_[0] == '\"') {
				exstack_.push_back(Atom(parse(op1_)[0] == parse(op2_)[0] == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(parse(op1_)[0] == std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stod(op1_) == std::stod(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) == std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) == std::stod(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stoi(op1_) == std::stoi(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) == std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) == std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}

	else if (rpn_[cnt_].val_ == "!=") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			exstack_.push_back(Atom(parse(op1_) != parse(op2_) == true ? "1" : "0", 0));
		}
		else if (op1_[0] == '\'') {
			if (op2_[0] == '\'' || op2_[0] == '\"') {
				exstack_.push_back(Atom(parse(op1_)[0] != parse(op2_)[0] == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(parse(op1_)[0] != std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stod(op1_) != std::stod(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) != std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) != std::stod(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (op2_[0] == '\'') {
				exstack_.push_back(Atom(std::stoi(op1_) != std::stoi(parse(op2_)) == true ? "1" : "0", 0));
			}
			else if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) != std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) != std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "&&") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			;
		}
		else if (op1_[0] == '\'') {
			;
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) && std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) && std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) && std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) && std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "||") {
		set_ops_bin();
		get_val(op1_);
		get_val(op2_);
		if (op1_[0] == '"') {
			;
		}
		else if (op1_[0] == '\'') {
			;
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stod(op1_) || std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stod(op1_) || std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		else {
			if (find(op2_.begin(), op2_.end(), '.') != op2_.end()) {
				exstack_.push_back(Atom(std::stoi(op1_) || std::stod(op2_) == true ? "1" : "0", 0));
			}
			else {
				exstack_.push_back(Atom(std::stoi(op1_) || std::stoi(op2_) == true ? "1" : "0", 0));
			}
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "!") {
		set_ops_uno();
		get_val(op1_);
		exstack_.push_back(Atom(std::to_string(factorial(std::stoi(op1_))), 0));
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "++") {
		if (op1_[0] == '\'') {
			exstack_.push_back(Atom(std::to_string(parse(op1_)[0] + 1), 0));
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			exstack_.push_back(Atom(std::to_string(std::stod(op1_) + 1), 0));
		}
		else {
			exstack_.push_back(Atom(std::to_string(std::stoi(op1_) + 1), 0));
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "--") {
		if (op1_[0] == '\'') {
			exstack_.push_back(Atom(std::to_string(parse(op1_)[0] - 1), 0));
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			exstack_.push_back(Atom(std::to_string(std::stod(op1_) - 1), 0));
		}
		else {
			exstack_.push_back(Atom(std::to_string(std::stoi(op1_) - 1), 0));
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "not") {
		set_ops_uno();
		get_val(op1_);
		if (!op1_.empty()) {
			exstack_.push_back(Atom("0", 0));
		}
		else {
			exstack_.push_back(Atom("1", 0));
		}
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "~") {
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "_") {
		set_ops_uno();
		get_val(op1_);
		if (op1_[0] == '\'') {
			exstack_.push_back(Atom(std::to_string(-(int)op1_[0]), 0));
		}
		else if (find(op1_.begin(), op1_.end(), '.') != op1_.end()) {
			exstack_.push_back(Atom(std::to_string(-std::stod(op1_)), 0));
		}
		else {
			exstack_.push_back(Atom(std::to_string(-std::stoi(op1_)), 0));
		}
		++cnt_;
	}
	else {
		throw; // unexpected operation
	}
} 


void Executor::Exec() {
	CTID_ = new TID(nullptr);
	FTID_ = CTID_;
	file.open("rpn.txt");
	out.open("output.txt");
	file >> size_;
	for (int i = 0; i < size_; ++i) {
		file >> val_ >> is_op_;
		if (val_ == "true") {
			val_ = "1";
		}
		else if (val_ == "false") {
			val_ = "0";
		}
		rpn_.push_back(Atom(val_, is_op_));

	}
	while (cnt_ < size_) {
		if (rpn_[cnt_].is_operator_) {
			Operate();
		}
		else {
			exstack_.push_back(rpn_[cnt_]);
			++cnt_;
		}
	}
}
