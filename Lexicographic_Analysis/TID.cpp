#include "TID.h"

void Local_TID::push_func(Func func, int flag = 0) {
	if (find_func(func.name) == "none") {
		funcs_[func.name] = func;
		if (flag == 1) {
			for (auto& i : func.params) {
				push_var(Var(i.first, i.second));
			}
		}
	}
	else {
		throw ErrorSemant("redeclaration occured"); // redeclaration
	}
}

void Local_TID::push_var(Var var) {
	if (find_var(var.name) == "none") {
		vars_[var.name] = var;
	}
	else {
		throw ErrorSemant("redeclaration occured"); // redeclaration
	}
}

std::string Local_TID::find_var(std::string var) {
	if (vars_.find(var) != vars_.end()) {
		return vars_[var].type;
	}
	return "none";
}

std::string Local_TID::find_func(std::string func) {
	if (funcs_.find(func) != funcs_.end()) {
		return funcs_[func].type;
	}
	return "none";
}

std::vector<std::pair<std::string, std::string> > Local_TID::get_func_params(std::string func) {
	Local_TID* tmp = this;
	while (tmp != nullptr) {
		if (tmp->funcs_.find(func) != tmp->funcs_.end()) {
			return tmp->funcs_[func].params;
		}
		else {
			tmp = tmp->parent_;
		}
	}
	return { {"none", "none"} };
}

std::string Local_TID::find_func_all(std::string func) {
	Local_TID* tmp = this;
	while (tmp != nullptr) {
		if (tmp->funcs_.find(func) != tmp->funcs_.end()) {
			return tmp->funcs_[func].type;
		}
		else {
			tmp = tmp->parent_;
		}
	}
	return "none";
}

std::string Local_TID::find_var_all(std::string var) {
	Local_TID* tmp = this;
	while (tmp != nullptr) {
		if (tmp->vars_.find(var) != tmp->vars_.end()) {
			return tmp->vars_[var].type;
		}
		else {
			tmp = tmp->parent_;
		}
	}
	return "none";
}

std::string TIDOperator::get_type_var(std::string name) {
	return cur->find_var_all(name);
}

std::string TIDOperator::get_type_func(std::string name) {
	return cur->find_func_all(name);
}



Local_TID* Local_TID::new_TID(Local_TID* parent) {
	next_ = new Local_TID(parent);
	return next_;
}

void Local_TID::del_TID(Local_TID* next) {
	delete next;
}

void TIDOperator::down() {
	cur = cur->new_TID(cur);
}

void TIDOperator::up() {
	cur = cur->parent_;
	cur->del_TID(cur->next_);
}

void TIDOperator::clear() {
	while (cur->parent_ != nullptr) {
		up();
	}
	delete cur;
}