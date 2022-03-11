#include "Syntactical.h"
#include "Semantical.h"

Syntactical::Syntactical(Lexical* lex) : lex_(lex), lexem_(Lex()), type_(lexem_.type), val_(lexem_.val) {
	try {
		ops_ = { "+", "-", "=", "*", "/", "%", "^", ">", "<", "**", "<=", ">=", "==", "+=", "-=", "*=", "/=", "|=", "%=", "^=", "&&", "||", "|", "&&", "=", "!=", "not"};
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
}

void Syntactical::check_op(int ch) {
	if (!ch) {
		return;
	}
	std::string op2 = pop();
	std::string sign = pop();
	if (ops_.find(op2) == ops_.end() && ops_.find(sign) == ops_.end()) {
		push(sign);
		push(op2);
		return;
	}
	std::string ret;
	if (op2 == "!") { // a! op2 and sign are swapped here
		ret = checker_->check_op(op2, sign);
		push(ret);
		return;
	}
	if (sign != "++" && sign != "--" && sign != "!" && sign != "not") {
		std::string op1 = pop();
		ret = checker_->check_op(sign, op1, op2);
		push(ret);
	}
	else {
		ret = checker_->check_op(sign, op2);
		push(ret);
	}
}

bool Syntactical::is_castable(std::string type1, std::string type2) {
	return cast_table_[type1][type2];
}

void Syntactical::push(std::string op) {
	stack_.push_back(op);
}

int Syntactical::check_ind() {
	int sz = stack_.size();
	if (sz <= 1) {
		return 0;
	}
	return 1;
}

std::string Syntactical::pop() {
	std::string tmp = stack_.back();
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


void Syntactical::Program() {
	while (true) {
		gl();
		if (val_ == "fun") {
			gl();
			if (type_ == 8) {
				std::string tmp = val_;
				typeo_ = lex_->lexems[lex_->index].val;
				gl();
				nameo_ = lex_->lexems[lex_->index].val;
				if (val_ != "main" && type_ == 2) {
					FuncDeclaration();
				}
				else if (val_ == "main" && tmp == "int") {
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
	namev_ = lex_->lexems[lex_->index].val;
	if (type_ == 2) {
		gl();
		if (val_ == "[") {
			low();
			while (true) {
				gl();
				if (val_ == "[") {
					typev_.push_back('[');
					Expression();
					gl();
					if (val_ == "]") {
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


			gl();
			if (val_ == "=") {
				gl();
				if (val_ == "{") {
					Array();
					gl();
					if (val_ == ";" || val_ == ",") {
						low();
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
				return;
			}
			else {
				throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ';'
			}
		}
		else {
			oper_->cur->push_var(Var(typev_, namev_));
			clear();
			if (val_ == "=") {
				std::string tmp = ReturnExpression();
				if (!is_castable(dec_type_, tmp)) {
					throw ErrorSemant(lexem_.str_number, "got " + tmp + " but expected " + dec_type_);
				}
			}
			else if (val_ == ";" || val_ == ",") {
				low();
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
	if (val_ == ";")
		return;
	if (type_ == 8) {
		low();
		Declaration(); // eats ;
	}
	else if (val_ == "cinout") {
		low();
		InOut();
		gl();
		if (val_ == ";") {
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
			return;
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, ";"); // expected ;
		}
	}


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
	while (val_ == ">>" || val_ == "<<") {
		Expression();
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
	push(oper_->cur->find_func_all(lex_->lexems[lex_->index].val));
	gl();
	if (val_ == "(") {
		Parameters();
		gl();
		if (val_ == ")") {
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


void Syntactical::Parameters() {
	gl();
	while (true) {
		if (val_ != ")") {
			low();
			Expression();
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
}



void Syntactical::DecFuncParameters() {
	gl();
	while (type_ == 8) {
		typev_ = lex_->lexems[lex_->index].val;
		gl();
		if (type_ == 2) {
			namev_ = lex_->lexems[lex_->index].val;
			params_.push_back(std::make_pair(typev_, namev_));
			Variable();
		}
		else {
			throw ErrorSynt(lexem_.str_number, val_, "identificator"); // expected name
		}
		gl();
		if (val_ == ",") {
			gl();
		}
	}
	if (val_ == ")") {
		return;
	}
	else {
		throw ErrorSynt(lexem_.str_number, val_, ")"); // expected ')'
	}
}

void Syntactical::Array() {
	gl();
	std::string prev = ",";
	while (val_ == "{") {
		Array();
		gl();
		if (val_ != "," && val_ != "}") {
			throw;
		}
		if (val_ == ",") {
			gl();
		}
		else {
			return;
		}
	}
	while (type_ == 3 || val_ == ",") {
		if (type_ == 3 && prev == ",") {
			prev = val_;
			gl();
		}
		else if (val_ == "," && prev != ",") {
			prev = val_;
			gl();
		}
		else {
			throw ErrorSynt(lexem_.str_number, val_, "value or ,");
		}
	}
	if (prev == ",") {
		throw ErrorSynt(lexem_.str_number, val_, "value or }");
	}
	if (val_ != "}") {
		throw  ErrorSynt(lexem_.str_number, val_, "}"); // expected }
	}
	return;
}

void Syntactical::Expression() {
	Atom10();
	gl();
	while (Priority10(val_)) {
		Atom10();
		check_op(check_ind());
		gl();
	}
	low();
}

bool Syntactical::Priority1(std::string st) {
	if (st == "++" || st == "--") {
		if (stack_.size() && stack_.back() != "--" && stack_.back() != "++" || !stack_.size())
			push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority2(std::string st) {
	if (st == "!") {
		if (stack_.size() && stack_.back() != "!" || !stack_.size())
			push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority3(std::string st) {
	if (st == "**") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority4(std::string st) {
	if (st == "*" || st == "/" || st == "%" || st == "$") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority5(std::string st) {
	if (st == "+" || st == "-") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority6(std::string st) {
	if (st == "^" || st == "|" || st == "&") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority7(std::string st) {
	if (st == "<" || st == ">" || st == "<=" || st == ">=" || st == "==" || st == "!=") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority8(std::string st) {
	if (st == "&&") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority9(std::string st) {
	if (st == "||") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority10(std::string st) {
	if (st == "=" || st == "+=" || st == "-=" || st == "*=" || st == "/="
		|| st == "%=" || st == "^=" || st == "$=" || st == "**=" || st == "&=" || st == "|=") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::PriorityNot(std::string st) {
	if (st == "not") {
		if (stack_.size() && stack_.back() != "not" || !stack_.size())
			push(st);
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
		Expression();
		gl();
		if (val_ == ")") {
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
	check_op(check_ind());
}

void Syntactical::Atom3() {
	Atom2();
	gl();
	while (Priority2(val_)) {
		gl();
	}
	check_op(check_ind());
	low();
}

void Syntactical::AtomNot() {
	Atom3();
	gl();
	while (Priority3(val_)) {
		Atom3();
		check_op(check_ind());
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
	check_op(check_ind());
}

void Syntactical::Atom5() {
	Atom4();
	gl();
	while (Priority4(val_)) {
		Atom4();
		check_op(check_ind());
		gl();
	}
	low();
}

void Syntactical::Atom6() {
	Atom5();
	gl();
	while (Priority5(val_)) {
		Atom5();
		check_op(check_ind());
		gl();
	}
	low();
}

void Syntactical::Atom7() {
	Atom6();
	gl();
	while (Priority6(val_)) {
		Atom6();
		check_op(check_ind());
		gl();
	}
	low();
}

void Syntactical::Atom8() {
	Atom7();
	gl();
	while (Priority7(val_)) {
		Atom7();
		check_op(check_ind());
		gl();
	}
	low();
}

void Syntactical::Atom9() {
	Atom8();
	gl();
	while (Priority8(val_)) {
		Atom8();
		check_op(check_ind());
		gl();
	}
	low();
}

void Syntactical::Atom10() {
	Atom9();
	gl();
	while (Priority9(val_)) {
		Atom9();
		check_op(check_ind());
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
		std::string tmp = ReturnExpression();
		if (is_castable(oper_->cur->func_type_, tmp)) {
			return;
		}
		else {
			throw ErrorSemant(lexem_.str_number, "returned " + tmp + " but expected " + oper_->cur->func_type_);
		}
	}
}

std::string Syntactical::ReturnExpression() {
	Expression();
	return stack_.back();
}

void Syntactical::Variable() { // DONE
	namev_ = lex_->lexems[lex_->index].val;
	gl();
	if (val_ == "(") {
		low();
		std::string tmp = oper_->cur->find_func_all(namev_);
		if (tmp == "none") {
			throw ErrorSemant(lexem_.str_number, "undefined function");; // undefined function
		}
		FuncCall();
		return;
	}
	while (val_ == "[") {
		Expression();
		gl();
		typev_.push_back('[');
		if (val_ == "]") {
			typev_.push_back(']');
			gl();
		}
		else {
			throw  ErrorSynt(lexem_.str_number, val_, "]"); // expected "]"
		}
	}
	push(typev_);
	low();
}

void Syntactical::Value() { // DONE without arrays
	gl();
	if (val_ == "{") {
		Array();
	}
	else if (type_ == 3) {
		if (find(val_.begin(), val_.end(), '"') != val_.end()) {
			push("string");
		}
		else if (find(val_.begin(), val_.end(), '\'') != val_.end()) {
			push("char");
		}
		else if (find(val_.begin(), val_.end(), '.') != val_.end()) {
			push("float");
		}
		else if (val_ == "true" || val_ == "false") {
			push("bool");
		}
		else {
			push("int");
		}
		return;
	}
	else if (val_ == "true" || val_ == "false") {
		push("bool");
	}
	else if (type_ == 4) {
		namev_ = lex_->lexems[lex_->index].val;
		gl();
		std::string tmp;
		if (val_ == "(") {
			low();
			tmp = oper_->cur->find_func_all(namev_);
			if (tmp == "none") {
				throw ErrorSemant(lexem_.str_number, "undefined function");; // undefined function
			}
			FuncCall();
		}
		else {
			low();
			if (oper_->cur->find_var_all(namev_) == "none") {
				throw ErrorSemant(lexem_.str_number, "undefined variable");; // undefined variable
			}
			Variable();
		}
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "value"); // unexpected value
	}
}