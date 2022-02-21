#include "Syntactical.h"

Syntactical::Syntactical(Lexical* lex) : lex_(lex), lexem_(Lex()), type_(lexem_.type), val_(lexem_.val) {
	Program();
}

void Syntactical::gl() {
	lexem_ = lex_->get_lex();
}

void Syntactical::low(int cnt = 1) {
	for (int i = 0; i < cnt; ++i)
		lex_->low();
}


void Syntactical::Program() {
	while (true) {
		gl();
		if (val_ == "fun") {
			gl();
			if (type_ == 8) {
				gl();
				if (val_ != "main" && type_ == 2) {
					FuncDeclaration();
				}
				else if (val_ == "main") {
					gl();
					if (val_ == "(") {
						gl();
						if (val_ == ")") {
							Block();
							return;
						}
						else {
							throw; // ')' expected
						}
					}
					else {
						throw; // '(' expected
					}
				}
				else {
					throw; // name expected
				}
			}
			else {
				throw; // type expected
			}
		}
		else if (type_ == 8) {
			low();
			Declaration();
		}
		else {
			throw; // expected declaration of func/variable
		}
	}
}

void Syntactical::FuncDeclaration() {
	gl();
	if (val_ == "(") {
		DecFuncParameters(); // also reads ')'
		Block();
	}
	else {
		throw; // '(' expected
	}
}

void Syntactical::Declaration() {
	gl();
	if (type_ == 8) {
		Section();
		gl();
		if (val_ == ";") {
			return;
		}
		else {
			throw; // expected ';'
		}
	}
	else {
		throw; // expected type
	}
}

void Syntactical::Section() {
	gl();
	if (type_ == 8) {
		gl();
		if (type_ == 2) {
			gl();
			if (val_ == "[") {
				low();
				while (true) {
					gl();
					if (val_ == "[") {
						gl();
						if (val_ == "int") {
							gl();
							if (val_ == "]") {
								continue;
							}
							else {
								throw; // expected ']'
							}
						}
						else {
							throw; // expected int
						}
					}
					else {
						low();
						break;
					}
				}
				gl();
				if (val_ == "=") {
					gl();
					if (val_ == "{") {
						Array();
						gl();
						if (val_ == ",") {
							Section();
						}
						else if (val_ == ";") {
							return;
						}
						else {
							throw; // expected ';' or ','
						}
					}
					else {
						low();
						Expression();
					}
				}
				else if (val_ == ";") {
					return;
				}
				else {
					throw; // expected ';'
				}
			}
			else if (val_ == "=") {
				Expression();
			}
			else if (val_ == ";") {
				return;
			}
			else {
				throw; // expected ";"
			}
		}
		else {
			throw; // expected name
		}
	}
	else {
		throw; // expected type
	}
}

void Syntactical::Block() {
	gl();
	if (val_ == "{") {
		Operator();
		gl();
		if (val_ == "}") {
			return;
		}
		else {
			throw; // expected '}'
		}
	}
	else {
		throw; // expected '{'
	}
}

void Syntactical::Operator() {
	
	gl();
	if (type_ == 8) {
		low();
		Declaration();
	}
	else if (val_ == "cinout") {
		low();
		InOut();
	}
	else if (val_ == "break" || val_ == "continue" || val_ == "return") {
		return;
	}
	else {
		low();
		Expression();
	}


	return;
}

void Syntactical::InOut() {
	gl();
	if (val_ == "cinout") {
		ListInOut();
	}
	else {
		throw; // expected cinout
	}
}

void Syntactical::ListInOut() {
	gl();
	if (val_ == ">>") {
		gl();
		if (type_ == 2) {
			gl();
			if (val_ == ";") {
				return;
			}
			else {
				throw; //expected ';'
			}
		}
		else {
			throw; // expected name (variable)
		}
	}
	else if (val_ == "<<") {
		gl();
		if (type_ == 2) {
			gl();
			if (val_ == ";") {
				return;
			}
			else {
				low(2);
				Expression();
			}
		}
		else {
			low();
			Expression();
		}
	}
	else {
		throw; // expected ">>" or "<<"
	}
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
			Block();
			gl();
			if (val_ == "else") {
				Block();
			}
			else {
				low();
				return;
			}
		}
		else {
			throw; // expected ')'
		}
	}
	else {
		throw; // expected '('
	}
}

void Syntactical::While() {
	gl();
	if (val_ == "(") {
		Expression();
		gl();
		if (val_ == ")") {
			Block();
		}
		else {
			throw; // expected ')'
		}
	}
	else {
		throw; // expected '('
	}
}

void Syntactical::DoWhile() {
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
					throw; // expected ';'
				}
			}
			else {
				throw; // expected ')'
			}
		}
		else {
			throw; // expected '('
		}
	}
	else {
		throw; // expected while operator
	}
}

void Syntactical::For() {
	gl();
	if (val_ == "(") {
		gl();
		if (type_ == 8) {
			low();
			Declaration();
			Expression();
			gl();
			if (val_ == ";") {
				Expression();
				gl();
				if (val_ == ")") {
					Block();
				}
				else {
					throw; // expected ')'
				}
			}
			else {
				throw; // expected ';'
			}
		}
		else {
			Expression();
			gl();
			if (val_ == ";") {
				Expression();
				gl();
				if (val_ == ";") {
					Expression();
					gl();
					if (val_ == ")") {
						Block();
					}
					else {
						throw; // expected ')'
					}
				}
				else {
					throw; // expected ';'
				}
			}
			else {
				throw; // expected ';'
			}
		}
	}
	else {
		throw; // expected '('
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
			throw; // expected ')'
		}
	}
	else {
		throw; // expected '('
	}
}

void Syntactical::Parameters() {
	gl();
	if (type_ == 2) {
		gl();
		if (val_ == "(") {
			low();
			FuncCall();
			gl();
			if (val_ == ",") {
				Parameters();
			}
			else if (val_ == ")") {
				low();
				return;
			}
			else {
				throw; // expected "," or ")"
			}
		}
		else {
			low(2);
			Expression();
			gl();
			if (val_ == ",") {
				Parameters();
			}
			else if (val_ == ")") {
				low();
				return;
			}
			else {
				throw; // expected "," or ")"
			}
		}
	}
	else if (val_ == ")") {
		low();
		return;
	}
	else {
		Expression();
		gl();
		if (val_ == ",") {
			Parameters();
		}
		else if (val_ == ")") {
			low();
			return;
		}
		else {
			throw; // expected "," or ")"
		}
	}
}

void Syntactical::DecFuncParameters() {
	gl();
	if (type_ == 8) {
		Variable();
	}
	else if (val_ == ")") {
		return;
	}
	else {
		throw; // expected ')'
	}
}

void Syntactical::Array() {
	gl();
	if (val_ == "}") {
		return;
	}
	else if (val_ == "{") {
		Array();
		gl();
		if (val_ == ",") {
			gl();
			if (val_ == "}") {
				return;
			}
			else if (val_ == "{") {
				Array();
			}
			else {
				throw; // expected '}' or '{'
			}
		}
		else if (val_ == "}") {
			return;
		}
		else {
			throw; // expected '}' or '{'
		}
	}
	else if (type_ == 8) {
		gl();
		while (val_ == ",") {
			gl();
			if (type_ == 8) {
				gl();
			}
			else if (val_ != "}" && type_ != 8) {
				throw; // expected '}'
			}
			else if (val_ == "}") {
				return;
			}
		}

	}
	else {
		throw; // expected '}' or '{'
	}
}

void Syntactical::Expression() {
	gl();
	if (type_ == 2) {
		low();
		Variable();
		gl();
		while (Priority10(val_)) {
			Atom10();
			gl();
		}
		low();
	}
	else {
		throw; // expected variable
	}
}

bool Syntactical::Priority1(std::string st) {
	if (st == "++" || st == "--") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority2(std::string st) {
	if (st == "!") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority3(std::string st) {
	if (st == "**") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority4(std::string st) {
	gl();
	if (st == "*" || st == "/" || st == "%" || st == "$") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority5(std::string st) {
	if (st == "+" || st == "-") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority6(std::string st) {
	if (st == "^" || st == "|" || st == "&") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority7(std::string st) {
	if (st == "<" || st == ">" || st == "<=" || st == ">=" || st == "==" || st == "!=") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority8(std::string st) {
	if (st == "&&") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority9(std::string st) {
	if (st == "||") {
		return true;
	}
	else {
		return false;
	}
}

bool Syntactical::Priority10(std::string st) {
	if (st == "=" || st == "+=" || st == "-=" || st == "*=" || st == "/="
		|| st == "%=" || st == "^=" || st == "$=" || st == "**=" || st == "&=" || st == "|=") {
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
		low();
		Variable();
	}
	else if (val_ == "(") {
		Expression();
		gl();
		if (val_ == ")") {
			return;
		}
		else {
			throw; // expected ')'
		}
	}
	else if (val_ == "not") {
		Expression();
	}
	else {
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

void Syntactical::Atom4() {
	Atom3();
	gl();
	while (Priority3(val_)) {
		Atom3();
		gl();
	}
	low();
}

void Syntactical::AtomNot() {
	gl();
	while (Priority4(val_)) {
		gl();
	}
	low();
	Atom4();
}

void Syntactical::Atom5() {
	AtomNot();
	gl();
	while (PriorityNot(val_)) {
		AtomNot();
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
