#include "Syntactical.h"
#include "Semantical.h"

Syntactical::Syntactical(Lexical* lex) 
	: lex_(lex)
	, lexem_(Lex())
	, type_(lexem_.type)
	, val_(lexem_.val)
	, max_depth_(-1)
	, index_(0)
	, call_(0)
	, decl_(0)
	, was_stopper_(0)
	, array_type_("none") {
	try {
		ops_ = { "+", "-", "=", "*", "/", "%", "^", ">", "<",
			"**", "<=", ">=", "==", "+=", "-=", "*=", "/=",
			"|=", "%=", "^=", "&&", "||", "|", "&&", "=", "!=", "not"};
		std::set<std::string> types = { "int", "string", "bool", "char", "float", "void" };
		oper_ = new TIDOperator();
		oper_->cur = new Local_TID(nullptr);
		checker_ = new ExpChecker();
		std::ifstream file("cast_table.txt");
		int n;
		std::string type1, type2 = "";
		for (auto& i : types) {
			for (auto& j : types) {
				cast_table_[i][j] = 0;
			}
		}
		file >> n;
		for (int i = 0; i < n; ++i) {
			file >> type1 >> type2;
			while (type2 != ";") {
				cast_table_[type1][type2] = 1;
				file >> type2;
			}
		}
		Program();
		std::cout << "Syntax: OK" << "\n";
	}
	catch (ErrorSynt err) {
		err.display();
	}
	catch (ErrorSemant err) {
		if (err.get_line_number() == -1) {
			err.set_line_number(lexem_.str_number);
		}
		err.display();
	}
	catch (...) {
		std::cout << "Unexpected behaviour\n";
	}
}

void Syntactical::check_op(int ch) {
	if (!ch) {
		return;
	}
	Type op2 = pop();
	Type sign = pop();
	if (ops_.find(op2.type) == ops_.end() && ops_.find(sign.type) == ops_.end() || sign.type == "(" || sign.index != op2.index || sign.call != op2.call) {
		push(sign.type, sign.index, sign.call);
		push(op2.type, op2.index, op2.call);
		return;
	}
	std::string ret;
	if (op2.type == "!") { // a! op2 and sign are swapped here
		ret = checker_->check_op(op2.type, sign.type);
		push(ret, op2.index, op2.call);
		return;
	}
	if (sign.type != "++" && sign.type != "--" && sign.type != "!" && sign.type != "not") {
		Type op1 = pop();
		if (op1.index != op2.index || op1.call != op2.call) {
			push(op1.type, op1.index, op1.call);
			push(sign.type, sign.index, sign.call);
			push(op2.type, op2.index, op2.call);
			return;
		}
		ret = checker_->check_op(sign.type, op1.type, op2.type);
		if (ret == "") {
			throw ErrorSemant(lexem_.str_number, "unexpected operands");
		}
		push(ret, op1.index, op1.call);
	}
	else {
		ret = checker_->check_op(sign.type, op2.type);
		if (ret == "") {
			throw ErrorSemant(lexem_.str_number, "unexpected operands");
		}
		push(ret, op2.index, op2.call);
	}
}

bool Syntactical::is_castable(std::string type1, std::string type2) {
	return cast_table_[type1][type2] || type1 == type2;
}

void Syntactical::push(std::string op, int ind, int call) {
	stack_.push_back(Type(op, ind, call));
}

int Syntactical::check_ind() {
	int sz = stack_.size();
	if (sz <= 1) {
		return 0;
	}
	return 1;
}

Type Syntactical::pop() {
	Type tmp = stack_.back();
	stack_.pop_back();
	return tmp;
}

void Syntactical::gl() {
	lexem_ = lex_->get_lex();
}

void Syntactical::low(int cnt = 1) {
	for (int i = 0; i < cnt; ++i)
		lex_->low();
}

void Syntactical::clear() {
	params_.clear();
}

void Syntactical::stack_clear() {
	stack_.clear();
}


void Syntactical::Program() {
	while (true) {
		params_.clear();
		gl();
		if (val_ == "fun") {
			gl();
			if (type_ == 8) {
				typeo_ = lex_->lexems[lex_->index].val;
				gl();
				while (val_ == "[") {
					typeo_ += "[";
					gl();
					if (val_ != "]") {
						throw ErrorSynt(lexem_.str_number, val_, "]");
					}
					typeo_ += "]";
					gl();
				}
				nameo_ = lex_->lexems[lex_->index].val;
				if (val_ != "main" && type_ == 2) {
					FuncDeclaration();
				}
				else if (val_ == "main" && typeo_ == "void") {
					gl();
					if (val_ == "(") {
						gl();
						if (val_ == ")") {
							oper_->cur->push_func(Func(typeo_, nameo_, params_), 0);
							oper_->down();
							oper_->cur->push_func(Func(typeo_, nameo_, params_), 1);
							Block();
							return;
						}
						else {
							throw ErrorSynt(lexem_.str_number, val_, ")"); // ')' expected
						}
					}
					else {
						throw ErrorSynt(lexem_.str_number, val_, "("); // '(' expected
					}
				}
				else {
					throw  ErrorSynt(lexem_.str_number, val_, "indentificator"); // name expected
				}
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, "type"); // type expected
			}
		}
		else if (type_ == 8) {
			low();
			Declaration();
		}
		else if (type_ == -1) {
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "declaration"); // expected declaration of func/variable
		}
	}
}

void Syntactical::FuncDeclaration() {
	gl();
	if (val_ == "(") {
		DecFuncParameters(); // also reads ')'
		oper_->cur->push_func(Func(typeo_, nameo_, params_), 0);
		oper_->down();
		oper_->cur->func_type_ = typeo_;
		oper_->cur->return_ = 1;
		oper_->cur->push_func(Func(typeo_, nameo_, params_), 1);
		Block();
		if (!was_stopper_ && typeo_ != "void") {
			throw ErrorSemant(lexem_.str_number, "expected " + typeo_ + " return but it wasn't detected");
		}
		was_stopper_ = 0;
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "("); // '(' expected
	}
}

void Syntactical::Declaration() {
	gl();
	if (type_ == 8) {
		typev_ = lex_->lexems[lex_->index].val;
		dec_type_ = typev_;
		Section();
		decl_ = 0;
		gl();
		if (val_ == ";") {
			return;
		}
		else {
			if (val_ != ",") {
				throw  ErrorSynt(lexem_.str_number, val_, ", or ;"); // expected "," || ";"
			}
			while (val_ == ",") {
				Section();
				decl_ = 0;
				gl();
			}
			if (val_ != ";") {
				throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ";"
			}
		}
	}
	else {
		throw ErrorSynt(lexem_.str_number, val_, "type"); // expected type
	}
}

void Syntactical::Section() {
	gl();
	decl_ = 1;
	namev_ = lex_->lexems[lex_->index].val;
	std::string tmp_type = typev_;
	if (type_ == 2) {
		gl();
		if (val_ == "[") {
			/*if (typev_ == "string" && decl_) {
				throw ErrorSemant(lexem_.str_number, "unexpected [");
			}*/
			low();
			while (true) {
				gl();
				if (val_ == "[") {
					++index_;
					typev_.push_back('[');
					std::string tp = ReturnExpression();
					if (!is_castable("int", tp)) {
						throw ErrorSemant(lexem_.str_number, tp + "can't be an index");
					}
					pop();
					gl();
					if (val_ == "]") {
						--index_ ;
						typev_.push_back(']');
						continue;
					}
					else {
						throw  ErrorSynt(lexem_.str_number, val_, "]"); // expected ']'
					}
				}
				else {
					low();
					break;
				}
			}
			oper_->cur->push_var(Var(typev_, namev_));
			clear();
			push(typev_, index_, call_);

			gl();
			decl_ = 0;
			if (val_ == "=") {
				push("=", index_, call_);
				gl();
				if (val_ == "{") {
					Array(tmp_type);
					gl();
					if (val_ == ";" || val_ == ",") {
						low();
						if (stack_.back().type != typev_) {
							throw ErrorSemant(lexem_.str_number, "uncompariable types: " + typev_ + " and " + stack_.back().type);
						}
						stack_clear();
						return;
					}
					else {
						throw  ErrorSynt(lexem_.str_number, val_, ", or ;"); // expected ';' or ','
					}
				}
				else {
					low();
					std::string tmp = ReturnExpression();
					if (!is_castable(dec_type_, tmp)) {
						throw ErrorSemant(lexem_.str_number, "got " + tmp + " but expected " + dec_type_);
					}
				}
			}
			else if (val_ == ";" || val_ == ",") {
				low();
				stack_clear();
				return;
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ';'
			}
		}
		else {
			oper_->cur->push_var(Var(typev_, namev_));
			clear();
			decl_ = 0;
			if (val_ == "=") {
				std::string tmp = ReturnExpression();
				if (!is_castable(dec_type_, tmp)) {
					throw ErrorSemant(lexem_.str_number, "got " + tmp + " but expected " + dec_type_);
				}
			}
			else if (val_ == ";" || val_ == ",") {
				low();
				stack_clear();
				return;
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, ", or ;"); // expected ";" or ","
			}
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "identificator"); // expected name
	}
}

void Syntactical::Block() {
	gl();
	if (val_ == "{") {
		gl();
		if (val_ == "}") {
			oper_->up();
			return;
		}
		else {
			low();
		}
		while (val_ != "}") {
			Operator();
			gl();
			if (val_ != "}")
				low();
		}
		if (val_ == "}") {
			oper_->up();
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "}"); // expected '}'
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "{"); // expected '{'
	}
}

void Syntactical::Operator() {
	
	gl();
	if (val_ == ";") {
		stack_clear();
		return;
	}
	if (type_ == 8) {
		low();
		Declaration(); // eats ;
	}
	else if (val_ == "cinout") {
		low();
		InOut();
		gl();
		if (val_ == ";") {
			stack_clear();
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ;
		}
	}
	else if (val_ == "break" || val_ == "continue" || val_ == "return") {
		Stopper();
		gl();
		if (val_ == ";") {
			stack_clear();
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ";"
		}
	}
	else if (val_ == "for" || val_ == "while" || val_ == "do" || val_ == "if") {
		DerivedOperator();
	}
	else {
		low();
		Expression();
		gl();
		if (val_ == ";") {
			stack_clear();
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ;
		}
	}

	stack_clear();
	return;
}

void Syntactical::InOut() {
	gl();
	if (val_ == "cinout") {
		ListInOut();
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "cinout"); // expected cinout
	}
}

void Syntactical::ListInOut() {
	gl();
	std::string tmp;
	while (val_ == ">>" || val_ == "<<") {
		stack_clear();
		tmp = val_;
		gl();
		if (tmp == ">>" && type_ != 2) {
			throw  ErrorSynt(lexem_.str_number, val_, "identificator"); // expected name
		}
		low();
		std::string tmp2 = ReturnExpression();
		if ((tmp2 == "void" || find(tmp2.begin(), tmp2.end(), '[') != tmp2.end()) && tmp == "<<") {
			throw ErrorSemant(lexem_.str_number, "unexpected type was pushed to print out");
		}
		gl();
	}
	low();
}

void Syntactical::DerivedOperator() {
	if (val_ == "for" || val_ == "while" || val_ == "do") {
		LoopOperator();
	}
	else {
		ConditionalOperator();
	}
}

void Syntactical::LoopOperator() {
	if (val_ == "for") {
		For();
	}
	else if (val_ == "while") {
		While();
	}
	else {
		DoWhile();
	}
}

void Syntactical::ConditionalOperator() {
	If();
}

void Syntactical::If() {
	gl();
	if (val_ == "(") {
		Expression();
		gl();
		if (val_ == ")") {
			oper_->down();
			Block();
			gl();
			if (val_ == "else") {
				oper_->down();
				Block();
			}
			else {
				low();
				return;
			}
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "("); // expected '('
	}
}

void Syntactical::While() {
	gl();
	if (val_ == "(") {
		Expression();
		gl();
		if (val_ == ")") {
			oper_->down();
			oper_->cur->stopper_ = 1;
			Block();
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "("); // expected '('
	}
}

void Syntactical::DoWhile() {
	oper_->down();
	oper_->cur->stopper_ = 1;
	Block();
	gl();
	if (val_ == "while") {
		gl();
		if (val_ == "(") {
			Expression();
			gl();
			if (val_ == ")") {
				gl();
				if (val_ == ";") {
					return;
				}
				else {
					throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ';'
				}
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
			}
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "("); // expected '('
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "while"); // expected while operator
	}
}

void Syntactical::For() {
	gl();
	oper_->down();
	oper_->cur->stopper_ = 1;
	if (val_ == "(") {
		gl();
		if (type_ == 8) {
			low();
			Declaration();
			Operator();
			gl();
			if (val_ == ")") {
				Block();
				return;
			}
			low();
			Expression();
			gl();
			if (val_ == ")") {
				Block();
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
			}
		}
		else {
			low();
			Operator();
			Operator();
			gl();
			if (val_ == ")") {
				Block();
				return;
			}
			low();
			Expression();
			gl();
			if (val_ == ")") {
				Block();
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
			}
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "("); // expected '('
	}
}

void Syntactical::FuncCall() { //DONE
	push(oper_->cur->find_func_all(lex_->lexems[lex_->index].val), index_, call_);
	++call_;
	std::vector<std::pair<std::string, std::string> > params = oper_->cur->get_func_params(lex_->lexems[lex_->index].val);
	gl();
	if (val_ == "(") {
		Parameters(params);
		gl();
		if (val_ == ")") {
			--call_;
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "("); // expected '('
	}
}


void Syntactical::Parameters(std::vector<std::pair<std::string, std::string> >& params) {
	gl();
	if (val_ == ")" && params.size()) {
		throw ErrorSemant(lexem_.str_number, "unexpected number of parameters in function call (less than expected)");
	}
	//TODO initiaalization of [][][] parameter
	int sz = params.size();
	int cnt = -1;
	std::string type;
	while (true) {
		if (val_ != ")") {
			low();
			type = ReturnExpression();
			pop();
			++cnt;
			if (cnt < sz) {
				if (sz && !is_castable(params[cnt].first, type)) {
					throw ErrorSemant(lexem_.str_number, "expected " + params[cnt].first + " but got " + type);
				}
			}
			else {
				throw ErrorSemant(lexem_.str_number, "unexpected number of parameters in function call (more than expected)");
			}
		}
		else if (val_ == ")") {
			low();
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "identificator or value"); // expected variable or value
		}
		gl();
		if (val_ == ",") {
			gl();
			if (type_ != 2 && type_ != 3 && val_ != "{") {
				throw  ErrorSynt(lexem_.str_number, val_, "identificator or value"); // expected variable or value
			}
		}
		else {
			low();
			break;
		}
	}
	if (cnt + 1 < sz) {
		throw ErrorSemant(lexem_.str_number, "unexpected number of parameters in function call (less than expected)");
	}
}



void Syntactical::DecFuncParameters() {
	gl();
	params_.clear();
	decl_ = 1;
	while (type_ == 8) {
		typev_ = lex_->lexems[lex_->index].val;
		gl();
		if (type_ == 2) {
			namev_ = lex_->lexems[lex_->index].val;
		}
		else {
			throw ErrorSynt(lexem_.str_number, val_, "identificator"); // expected name
		}
		gl();
		while (val_ == "[") {
			typev_ += "[";
			gl();
			if (val_ != "]") {
				throw ErrorSynt(lexem_.str_number, val_, "]");
			}
			typev_ += "]";
			gl();
		}
		params_.push_back(std::make_pair(typev_, namev_));
		if (val_ == ",") {
			gl();
		}
	}
	decl_ = 0;
	if (val_ == ")") {
		return;
	}
	else {
		throw ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
	}
}


int Syntactical::array_check(int depth = 1) {
	if (val_ == "{") {
		max_depth_ = std::max(max_depth_, array_check(depth + 1));
	}
	if (val_ == "}") {
		return depth;
	}
}


std::string Syntactical::Array(std::string array_type) {
	std::set<int> se;
	int depth = 1;
	max_depth_ = -1;
	std::string tmp;
	bool flag = 1;
	while (true) {
		gl();
		if (val_ == "{") {
			++depth;
			flag = 1;
		}
		else if (val_ == ",") {
			continue;
		}
		else if (val_ == "}") {
			if (depth == 1) {
				if (max_depth_ == -1) {
					max_depth_ = depth;
				}
				break;
			}
			if (max_depth_ != -1 && flag) {
				if (depth != max_depth_) {
					throw ErrorSemant(lexem_.str_number, "different depths");
				}
			}
			else {
				if (flag) {
					max_depth_ = depth;
				}
			}
			flag = 0;
			--depth;
		}
		else {
			low();
			tmp = ReturnExpression();
			pop();
			if (array_type == "none") {
				array_type = tmp;
			}
			if (tmp != array_type) {
				throw ErrorSemant(lexem_.str_number, "unexpected type of element: got " + tmp + " but " + array_type + " expected");
			}
		}
	}
	for (int i = 0; i < max_depth_; ++i) {
		array_type += "[]";
	}
	push(array_type, index_, call_);
	return array_type;

}



void Syntactical::Expression() {
	Atom10();
	gl();
	while (Priority10(val_)) {
		Atom10();
		while (stack_.size() > 1 && Priority10(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

bool Syntactical::Priority1(std::string st) {
	if (st == "++" || st == "--") {
		if (stack_.size() && stack_.back().type != "--" && stack_.back().type != "++" || !stack_.size())
			push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority2(std::string st) {
	if (st == "!") {
		if (stack_.size() && stack_.back().type != "!" || !stack_.size())
			push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority3(std::string st) {
	if (st == "**") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority4(std::string st) {
	if (st == "*" || st == "/" || st == "%" || st == "$") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority5(std::string st) {
	if (st == "+" || st == "-") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority6(std::string st) {
	if (st == "^" || st == "|" || st == "&") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority7(std::string st) {
	if (st == "<" || st == ">" || st == "<=" || st == ">=" || st == "==" || st == "!=") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority8(std::string st) {
	if (st == "&&") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority9(std::string st) {
	if (st == "||") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority10(std::string st) {
	if (st == "=" || st == "+=" || st == "-=" || st == "*=" || st == "/="
		|| st == "%=" || st == "^=" || st == "$=" || st == "**=" || st == "&=" || st == "|=") {
		push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::PriorityNot(std::string st) {
	if (st == "not") {
		if (stack_.size() && stack_.back().type != "not" || !stack_.size())
			push(st, index_, call_);
		return true;
	}
	else {
		return false;
	}
}

void Syntactical::Atom1() { //DONE
	gl();
	if (type_ == 2) {
		if (oper_->cur->find_var_all(lex_->lexems[lex_->index].val) == "none") {
			gl();
			if (val_ != "(") {
				throw ErrorSemant(lexem_.str_number, "undefined variable"); // undefined variable
			}
			low();
		}
		Variable();
	}
	else if (val_ == "(") {
		push("(", index_, call_);
		Expression();
		gl();
		if (val_ == ")") {
			stack_[stack_.size() - 2] = stack_[stack_.size() - 1];
			pop();
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
		}
	}
	else {
		low();
		Value();
	}
}

void Syntactical::Atom2() {
	gl();
	while (Priority1(val_)) {
		gl();
	}
	low();
	Atom1();

	while (stack_.size() > 1 && Priority1(stack_[stack_.size() - 2].type)) {
		pop();
		check_op(check_ind());
	}
}

void Syntactical::Atom3() {
	Atom2();
	gl();
	while (Priority2(val_)) {
		gl();
	}
	while (stack_.size() > 1 && Priority2(stack_[stack_.size() - 2].type)) {
		pop();
		check_op(check_ind());
	}
	low();
}

void Syntactical::AtomNot() {
	Atom3();
	gl();
	while (Priority3(val_)) {
		Atom3();
		while (stack_.size() > 1 && Priority3(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Atom4() {
	gl();
	while (PriorityNot(val_)) {
		gl();
	}
	low();
	AtomNot();
	while (stack_.size() > 1 && PriorityNot(stack_[stack_.size() - 2].type)) {
		pop();
		check_op(check_ind());
	}
}

void Syntactical::Atom5() {
	Atom4();
	gl();
	while (Priority4(val_)) {
		Atom4();
		while (stack_.size() > 1 && Priority4(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Atom6() {
	Atom5();
	gl();
	while (Priority5(val_)) {
		Atom5();
		while (stack_.size() > 1 && Priority5(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Atom7() {
	Atom6();
	gl();
	while (Priority6(val_)) {
		Atom6();
		while (stack_.size() > 1 && Priority6(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Atom8() {
	Atom7();
	gl();
	while (Priority7(val_)) {
		Atom7();
		while (stack_.size() > 1 && Priority7(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Atom9() {
	Atom8();
	gl();
	while (Priority8(val_)) {
		Atom8();
		while (stack_.size() > 1 && Priority8(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Atom10() {
	Atom9();
	gl();
	while (Priority9(val_)) {
		Atom9();
		while (stack_.size() > 1 && Priority9(stack_[stack_.size() - 2].type)) {
			pop();
			check_op(check_ind());
		}
		gl();
	}
	low();
}

void Syntactical::Stopper() {

	if (val_ != "break" && val_ != "continue" && val_ != "return") {
		throw ErrorSynt(lexem_.str_number, val_, "return, break or continue");
	}

	if ((val_ == "break" || val_ == "continue") && !oper_->cur->stopper_) {
		throw ErrorSemant(lexem_.str_number, val_ + " is not allowed");
	}

	if (val_ == "return" && !oper_->cur->return_) {
		throw ErrorSemant(lexem_.str_number, "return is not allowed");
	}

	if (val_ == "return") {
		was_stopper_ = 1;
		gl();
		if (oper_->cur->func_type_ == "void" && val_ != ";") {
			throw ErrorSemant(lexem_.str_number, "returned something but expected void return");
		}
		low();
		if (oper_->cur->func_type_ == "void") {
			stack_clear();
			return;
		}
		gl();
		std::string tmp;
		if (val_ == "{") {
			tmp = Array();
		}
		else {
			low();
			tmp = ReturnExpression();
		}
		if (is_castable(oper_->cur->func_type_, tmp)) {
			stack_clear();
			return;
		}
		else {
			throw ErrorSemant(lexem_.str_number, "returned " + tmp + " but expected " + oper_->cur->func_type_);
		}
	}
}

std::string Syntactical::ReturnExpression() {
	Expression();
	return stack_.back().type;
}

void Syntactical::Variable() { // DONE
	namev_ = lex_->lexems[lex_->index].val;
	gl();
	if (val_ == "(") {
		low();
		std::string tmp = oper_->cur->find_func_all(namev_);
		if (tmp == "none") {
			throw ErrorSemant(lexem_.str_number, "undefined function"); // undefined function
		}
		FuncCall();
		return;
	}
	typev_ = oper_->get_type_var(namev_);
	if (typev_ == "string" && val_ == "[") {
		if (decl_) {
			throw ErrorSemant(lexem_.str_number, "undefined [");
		}
		++index_;
		Expression();
		gl();
		if (val_ == "]") {
			pop();
			--index_;
			push("char", index_, call_);
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "]");
		}
	}
	std::string type_cur;
	while (val_ == "[") {
		if (decl_) {
			throw ErrorSemant(lexem_.str_number, "unexpected [");
		}
		++index_;
		std::string tp = ReturnExpression();
		if (!is_castable("int", tp)) {
			throw ErrorSemant(lexem_.str_number, tp + "can't be an index");
		}
		pop();
		gl();
		type_cur.push_back('[');
		if (val_ == "]") {
			type_cur.push_back(']');
			--index_;
			gl();
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "]"); // expected "]"
		}
	} // int a[][] -> int
	int count_brackets_essential = count(typev_.begin(), typev_.end(), '[');
	int res_brackets = count_brackets_essential - count(type_cur.begin(), type_cur.end(), '[');
	std::string type_return;
	for (auto& i : typev_) {
		if (i != '[') {
			type_return.push_back(i);
		}
		else {
			break;
		}
	}
	for (int i = 0; i < res_brackets; ++i) {
		type_return.push_back('[');
		type_return.push_back(']');

	}
	push(type_return, index_, call_);
	low();
	decl_ = 0;
}

void Syntactical::Value() { // DONE 
	/*if (val_ == "[") {
		index_ = 1;
	}*/
	gl();
	if (val_ == "{" && !decl_) {
		Array();
	}
	else if (type_ == 3 && !decl_ || type_ == 3 && index_) {
		if (find(val_.begin(), val_.end(), '"') != val_.end()) {
			push("string", index_, call_);
		}
		else if (find(val_.begin(), val_.end(), '\'') != val_.end()) {
			push("char", index_, call_);
		}
		else if (find(val_.begin(), val_.end(), '.') != val_.end()) {
			push("float", index_, call_);
		}
		else if (val_ == "true" || val_ == "false") {
			push("bool", index_, call_);
		}
		else {
			push("int", index_, call_);
		}
		return;
	}
	else if ((val_ == "true" || val_ == "false") && !decl_) {
		push("bool", index_, call_);
	}
	else if (type_ == 4) {
		namev_ = lex_->lexems[lex_->index].val;
		gl();
		std::string tmp;
		if (val_ == "(") {
			low();
			tmp = oper_->cur->find_func_all(namev_);
			if (tmp == "none") {
				throw ErrorSemant(lexem_.str_number, "undefined function"); // undefined function
			}
			FuncCall();
		}
		else {
			low();
			if (oper_->cur->find_var_all(namev_) == "none") {
				throw ErrorSemant(lexem_.str_number, "undefined variable"); // undefined variable
			}
			Variable();
			decl_ = 0;
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "value"); // unexpected value
	}
}