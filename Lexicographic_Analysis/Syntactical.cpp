#include "Syntactical.h"
#include "Semantical.h"

Syntactical::Syntactical(Lexical* lex) : lex_(lex), lexem_(Lex()), type_(lexem_.type), val_(lexem_.val) {
	try {
		ops_ = { "+", "-", "=", "*", "/", "%", "^", ">", "<", "**", "<=", ">=", "==", "+=", "-=", "*=", "/=", "|=", "%=", "^=", "&&", "||", "|", "&&", "=", "!=" };
		oper_ = new TIDOperator();
		oper_->cur = new Local_TID(nullptr);

		Program();
		std::cout << "Syntax: OK" << "\n";
	}
	catch (ErrorSynt err) {
		err.display();
	}
	catch (ErrorSemant err) {
		err.display();
	}
}

void Syntactical::check_op(int ch) {
	if (!ch) {
		return;
	}
	std::string op2 = pop();
	std::string sign = pop();
	if (sign != "++" && sign != "--" && sign != "!"){
		std::string op1 = pop();
		checker_->check_op(sign, op1, op2);
	}
	else {
		checker_->check_op(sign, op2);
	}
}

void Syntactical::push(std::string op) {
	stack_.push_back(op);
}

int Syntactical::check_ind() {
	int sz = stack_.size();
	if (sz == 1) {
		return 0;
	}
	if (find(ops_.begin(), ops_.end(), stack_[sz - 2]) != ops_.end()) {
		return 1;
	}
	return 0;
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
		throw; // expected type
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
					Expression();
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
				Expression();
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

void Syntactical::FuncCall() {
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

/*void Syntactical::Parameters() {
	gl();
	while (true) {
		if (type_ == 2) {
			Variable();
		}
		else if (type_ == 3 || val_ == "{") {
			if (val_ == "{") {
				low();
			}
			Value();
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
}*/


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
		gl();
	}
	low();
}

bool Syntactical::Priority1(std::string st) {
	if (st == "++" || st == "--") {
		push(st);
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority2(std::string st) {
	if (st == "!") {
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
		return true;
	}
	else {
		return false;
	}
}

void Syntactical::Atom1() {
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
}

void Syntactical::Atom3() {
	Atom2();
	gl();
	while (Priority2(val_)) {
		gl();
	}
	low();
}

void Syntactical::AtomNot() {
	Atom3();
	gl();
	while (Priority3(val_)) {
		Atom3();
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
}

void Syntactical::Atom5() {
	Atom4();
	gl();
	while (Priority4(val_)) {
		Atom4();
		gl();
	}
	low();
}

void Syntactical::Atom6() {
	Atom5();
	gl();
	while (Priority5(val_)) {
		Atom5();
		gl();
	}
	low();
}

void Syntactical::Atom7() {
	Atom6();
	gl();
	while (Priority6(val_)) {
		Atom6();
		gl();
	}
	low();
}

void Syntactical::Atom8() {
	Atom7();
	gl();
	while (Priority7(val_)) {
		Atom7();
		gl();
	}
	low();
}

void Syntactical::Atom9() {
	Atom8();
	gl();
	while (Priority8(val_)) {
		Atom8();
		gl();
	}
	low();
}

void Syntactical::Atom10() {
	Atom9();
	gl();
	while (Priority9(val_)) {
		Atom9();
		gl();
	}
	low();
}

void Syntactical::Stopper() {
	gl();
	if (val_ == "break" || val_ == "continue" || val_ == "return") {
		return;
	}
	else {
		throw  ErrorSynt(lexem_.str_number, val_, "return, break or continue"); // expected stopper
	}
}

void Syntactical::Variable() {
	namev_ = lex_->lexems[lex_->index].val;
	gl();
	if (val_ == "(") {
		low();
		if (oper_->cur->find_func_all(namev_) == "none") {
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
	//push(oper_->get_type_var(namev_));
	//check_op(check_ind());
	low();
}

void Syntactical::Value() {
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
		//check_op(check_ind());
		return;
	}
	else if (type_ == 4) {
		namev_ = lex_->lexems[lex_->index].val;
		gl();
		if (val_ == "(") {
			low();
			if (oper_->cur->find_func_all(namev_) == "none") {
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