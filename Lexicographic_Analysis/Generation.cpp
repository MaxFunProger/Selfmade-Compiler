#include "Generation.h"

void Generation::Generator() {
	for (auto cur_lex = lex_->get_lex(); cur_lex.type != -1; cur_lex = lex_->get_lex()) {
		if (cur_lex.val == "fun") {
			gen_fun();
		}
		else {
			lex_->low();
			gen_declaration();
		}
	}
	print();
}

void Generation::print() {
	for (auto& x : result) {
		std::cout << x.val_ << " " << x.is_operator_ << "\n";
	}
}

void Generation::add_to_result(Atom a) {
	result.push_back(a);
}

void Generation::gen_fun() {
	Atom f_type = Atom(lex_->get_lex(), 0);
	Atom f_name = Atom(lex_->get_lex(), 0);
	bool is_main = f_name.val_ == "main";
	add_to_result(f_name);
	add_to_result(f_type);
	add_to_result(Atom("create_fun", 1));
	int skip_f = result.size();
	if (!is_main) {
		add_to_result(Atom("-1", 0));
		add_to_result(Atom("goto", 1));
	}
	std::vector <Atom> rev;
	for (Lex cur = lex_->get_lex(); cur.val != ")"; cur = lex_->get_lex()) {
		if (cur.type < 3) {
			rev.push_back(Atom(cur, 0));
		}
	}
	std::reverse(rev.begin(), rev.end());
	for (int i = 1; i < rev.size(); i += 2) {
		add_to_result(rev[i - 1]);  // name
		add_to_result(rev[i]);		// type
		add_to_result(Atom("create_var", 1));
	}
	gen_block();
	if (!is_main) {
		result[skip_f].val_ = std::to_string(result.size());
	}
}

void Generation::gen_declaration() {
	Atom var_type = Atom(lex_->get_lex(), 0);
	Lex cur = lex_->get_lex();
	while (cur.val != ";") {
		Atom var_name = Atom(cur, 0);
		Lex sth = lex_->get_lex();
		gen_expression();
		if (sth.val == "(") {
			lex_->get_lex(); // skip closing bracket
		}
		add_to_result(var_name);
		add_to_result(var_type);
		add_to_result(Atom("create_var", 1));
		cur = lex_->get_lex();
	}
}

void Generation::gen_block() {
	add_to_result(Atom("create_tid", 1));
	lex_->get_lex(); // skip {
	Lex cur = lex_->get_lex();
	while (cur.val != "}") {
		if (cur.val == "return") {
			gen_return();
		}
		else if (cur.val == "break") {
			gen_break();
		}
		else if (cur.val == "continue") {
			gen_continue();
		}
		else if (cur.val == "cinout") {
			gen_cinout();
		}
		else if (cur.val == "if") {
			gen_if();
		}
		else if (cur.val == "while") {
			gen_while();
		}
		else if (cur.val == "for") {
			gen_for();
		}
		else if (cur.val == "do") {
			gen_dowhile();
		}
		else if (cur.type == 8) {
			lex_->low();
			gen_declaration();
		}
		else {
			lex_->low();
			gen_expression();
			add_to_result(Atom("clear_oparand_stack", 1));
			lex_->get_lex(); // skip ;
		}
		cur = lex_->get_lex();
	}
}

void Generation::gen_return() {
	if (lex_->get_lex().val == ";") {
		add_to_result(Atom("0", 0));
		add_to_result(Atom("return", 1));
	}
	else {
		lex_->low();
		gen_expression();
		add_to_result(Atom("return", 1));
		lex_->get_lex(); // skip ;
	}
}

void Generation::gen_break() {
	cycle_stack.back().breaks.push_back(result.size());
	add_to_result(Atom("-1", 0));
	add_to_result(Atom("goto", 1));
	lex_->get_lex(); // skip ;
}

void Generation::gen_continue() {
	add_to_result(Atom(std::to_string(cycle_stack.back().begin), 0));
	add_to_result(Atom("goto", 1));
	lex_->get_lex(); // skip ;
}

void Generation::gen_cinout() {
	for (Lex cur = lex_->get_lex(); cur.val != ";"; cur = lex_->get_lex()) {
		gen_expression();
		if (cur.val == "<<") {
			add_to_result(Atom("print", 1));
		}
		else {
			add_to_result(Atom("scan", 1));
		}
	}
}

void Generation::gen_if() {
	lex_->get_lex(); // skip (
	gen_expression();
	lex_->get_lex(); // skip }
	int p2id = result.size();
	add_to_result(Atom("-1", 0));
	add_to_result(Atom("if_not_goto", 1));
	gen_block();
	if (lex_->get_lex().val == "else") {
		int p3id = result.size();
		add_to_result(Atom("-1", 0));
		add_to_result(Atom("goto", 1));
		result[p2id].val_ = std::to_string(result.size());
		gen_block();
		result[p3id].val_ = std::to_string(result.size());
	}
	else {
		lex_->low();
		result[p2id].val_ = std::to_string(result.size());
	}
}

void Generation::gen_while() {
	int pe = result.size();
	cycle_stack.emplace_back(pe);
	lex_->get_lex(); // skip (
	gen_expression();
	lex_->get_lex(); // skip )

	int p2id = result.size();
	add_to_result(Atom("-1", 0));
	add_to_result(Atom("if_not_goto", 1));
	gen_block();
	add_to_result(Atom(std::to_string(pe), 0));
	add_to_result(Atom("goto", 1));
	for (auto& x : cycle_stack.back().breaks) {
		result[x].val_ = std::to_string(result.size());
	}
	cycle_stack.pop_back();
	result[p2id].val_ = std::to_string(result.size());
}

void Generation::gen_dowhile() {
	int p1 = result.size();
	cycle_stack.emplace_back(p1);
	gen_block();
	lex_->get_lex(); // skip while
	lex_->get_lex(); // skip (
	gen_expression();
	lex_->get_lex(); // skip )
	add_to_result(Atom("not", 1));
	add_to_result(Atom(std::to_string(p1), 0));
	add_to_result(Atom("if_not_goto", 1));
	for (auto& x : cycle_stack.back().breaks) {
		result[x].val_ = std::to_string(result.size());
	}
	cycle_stack.pop_back();
	lex_->get_lex(); // skip ;
}

void Generation::gen_for() {
	lex_->get_lex(); // skip (
	if (lex_->get_lex().type == 8) {
		lex_->low();
		gen_declaration();
	}
	else {
		lex_->low();
		gen_expression();
		add_to_result(Atom("clear_oparand_stack", 1));
		lex_->get_lex(); // skip ;
	}

	int p2 = result.size();
	gen_expression();
	lex_->get_lex(); // skip ;

	int p5id = result.size();
	add_to_result(Atom("-1", 0));
	add_to_result(Atom("if_not_goto", 1));

	int p4id = result.size();
	add_to_result(Atom("-1", 0));
	add_to_result(Atom("goto", 1));

	int p3 = result.size();
	cycle_stack.emplace_back(p3);
	gen_expression();
	lex_->get_lex(); // skip )

	add_to_result(Atom(std::to_string(p2), 0));
	add_to_result(Atom("goto", 1));

	result[p4id].val_ = std::to_string(result.size());
	gen_block();
	add_to_result(Atom(std::to_string(p3), 0));
	add_to_result(Atom("goto", 1));

	result[p5id].val_ = std::to_string(result.size());
	for (auto& x : cycle_stack.back().breaks) {
		result[x].val_ = std::to_string(result.size());
	}
	cycle_stack.pop_back();
}

void Generation::gen_expression() {
	int balance = 0;
	for (Lex cur = lex_->get_lex(); cur.val != ";" && cur.val != ","; cur = lex_->get_lex()) {
		if (cur.val == ")" && balance == 0) {
			break;
		}
		if (cur.val == "not" || cur.type == 4) {
			if (right_assoc.count(cur.val)) {
				while (!operator_stack.empty() && priority[operator_stack.back().val_] < priority[cur.val]) {
					result.push_back(operator_stack.back());
					operator_stack.pop_back();
				}
				operator_stack.push_back(Atom(cur.val, 1));
			}
			else {
				while (!operator_stack.empty() && priority[operator_stack.back().val_] <= priority[cur.val]) {
					result.push_back(operator_stack.back());
					operator_stack.pop_back();
				}
				operator_stack.push_back(Atom(cur.val, 1));
			}
			continue;
		}
		if (cur.val == "(") {
			++balance;
			operator_stack.push_back(Atom(cur.val, 1));
			continue;
		}
		if (cur.val == "[") {
			++balance;
			operator_stack.push_back(Atom(cur.val, 1));
			continue;
		}
		if (cur.val == ")") {
			--balance;
			while (!operator_stack.empty() && operator_stack.back().val_ != "(") {
				result.push_back(operator_stack.back());
				operator_stack.pop_back();
			}
			operator_stack.pop_back();
			continue;
		}
		if (cur.val == "]") {
			--balance;
			while (!operator_stack.empty() && operator_stack.back().val_ != "[") {
				result.push_back(operator_stack.back());
				operator_stack.pop_back();
			}
			operator_stack.pop_back();
			result.push_back(Atom("index", 1));
			continue;
		}
		if (lex_->get_lex().val == "(") {
			do {
				gen_expression();
			} while (lex_->get_lex().val == ",");
			result.push_back(Atom(cur.val, 1));
		}
		else {
			lex_->low();
			result.push_back(Atom(cur.val, 0));
		}
	}
	while (!operator_stack.empty()) {
		result.push_back(operator_stack.back());
		operator_stack.pop_back();
	}
	lex_->low();
}