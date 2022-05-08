#include "Execution.h"



void Executor::Add_TID() {
	CTID_->next = new TID(CTID_);
	FTID_ = CTID_ = CTID_->next;
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
	if (FTID_->var_type_.count(name) != 0) {
		LTID_ = FTID_;
		FTID_ = CTID_;
		return LTID_;
	}
	else {
		Up_TID();
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
		out << exstack_.back().val_;
		exstack_.pop_back();
		++cnt_;
	}
	else if (rpn_[cnt_].val_ == "scan") {
		std::string var = exstack_.back().val_;
		exstack_.pop_back();
		Find_Var_TID(var);
		if (LTID_->var_type_[var] == "int") {
			in >> LTID_->var_int_[var];
		}
		else if (LTID_->var_type_[var] == "bool") {
			in >> LTID_->var_bool_[var];
		}
		else if (LTID_->var_type_[var] == "char") {
			in >> LTID_->var_char_[var];
		}
		else if (LTID_->var_type_[var] == "string") {
			in >> LTID_->var_string_[var];
		}
		else if (LTID_->var_type_[var] == "float") {
			in >> LTID_->var_float_[var];
		}
		++cnt_;
	} // TODO * - + etc.
}


void Executor::Exec() {
	CTID_ = new TID(nullptr);
	FTID_ = CTID_;
	file.open("rpn.txt");
	out.open("output.txt");
	file >> size_;
	for (int i = 0; i < size_; ++i) {
		file >> val_ >> is_op_;
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
