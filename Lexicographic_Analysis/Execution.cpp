#include "Execution.h"



bool TID::get_bool(std::string name) {
	return var_bool_[name];
}

int TID::get_int(std::string name) {
	return var_int_[name];
}

char TID::get_char(std::string name) {
	return var_char_[name];
}

std::string TID::get_string(std::string name) {
	return var_string_[name];
}

double TID::get_float(std::string name) {
	return var_float_[name];
}

void Executor::Exec() {
	CTID_ = new TID(nullptr);
	FTID_ = CTID_;
	while (cnt_ < size_) {

	}
}

void Executor::Add_TID() {
	CTID_->next = new TID(CTID_);
	CTID_ = CTID_->next;
}

void Executor::Del_TID() {
	CTID_ = CTID_->prev;
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
		return nullptr;
	}
	if (FTID_->var_type_.count(name) != 0) {
		LTID = FTID_;
		FTID_ = CTID_;
		return LTID;
	}
	else {
		Up_TID();
	}
}

void Executor::Add_Var_Int(std::string type, std::string name, int val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_int_[name] = val;
}

void Executor::Add_Var_Bool(std::string type, std::string name, bool val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_bool_[name] = val;
}

void Executor::Add_Var_Char(std::string type, std::string name, char val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_char_[name] = val;
}

void Executor::Add_Var_String(std::string type, std::string name, std::string val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_string_[name] = val;
}

void Executor::Add_Var_Float(std::string type, std::string name, double val)
{
	CTID_->var_type_[name] = type;
	CTID_->var_float_[name] = val;
}

int Executor::Get_Var_Int(std::string name)
{
	return LTID->var_int_[name];
}

bool Executor::Get_Var_Bool(std::string name)
{
	return LTID->var_bool_[name];
}

char Executor::Get_Var_Char(std::string name)
{
	return LTID->var_char_[name];
}

std::string Executor::Get_Var_String(std::string name)
{
	return LTID->var_string_[name];
}

double Executor::Get_Var_Float(std::string name)
{
	return LTID->var_float_[name];
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

}


void Executor::Exec() {
	std::ifstream file("rpn.txt");
	file >> size_;
	for (int i = 0; i < size_; ++i) {
		file >> val_ >> is_op_;
		rpn_.push_back(Atom(val_, is_op_));
		if (rpn_.back().is_operator_) {
			Operate();
		}
	}
}