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
		return nullptr;
	}
	if (FTID_->is_visible_[name]) {
		return FTID_;
	}
	else {
		Up_TID();
	}
}

TID* Executor::Find_Func_TID(std::string name) {
	if (FTID_ == nullptr) {
		return nullptr;
	}
	if (FTID_->is_visible_[name]) {
		return FTID_;
	}
	else {
		Up_TID();
	}
}

