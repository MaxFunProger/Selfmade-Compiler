#include "Lexical.h"

Lexical::Lexical() {
	std::ifstream file("stuff.txt");
	std::ifstream mfile("machine.txt");
	std::ifstream input("input.txt");

	int n;
	std::string st;
	file >> n;
	for (int i = 0; i < n; ++i) {
		file >> st;
		stuff.insert(st);
	}
	file >> n;
	for (int i = 0; i < n; ++i) {
		file >> st;
		separators.insert(st);
	}
	file >> n;
	for (int i = 0; i < n; ++i) {
		file >> st;
		operators.insert(st);
	}

	states.resize(50, nullptr);

	mfile >> n;
	int id, term, id2, lex;
	for (int i = 0; i < n; ++i) {
		mfile >> id >> st >> term >> lex;
		states[id] = new State;
		states[id]->type = st;
		states[id]->is_terminal = term;
		states[id]->trans.resize(128, nullptr);
		result[st] = lex;
	}

	mfile >> n;
	for (int i = 0; i < n; ++i) {
		mfile >> id >> id2;
		if (id == 1 && id2 == 1 || id == 4 && id2 == 5) {
			for (int j = 32; j <= 126; ++j) {
				if (j == 34 || j == 39)
					continue;
				states[id]->trans[j] = states[id2];
			}
			char ha;
			mfile >> id >> id >> ha;
			continue;
		}
		char tmp = '1';
		while (tmp != '@') {
			mfile >> tmp;
			if (tmp == ' ')
				continue;
			states[id]->trans[(int)tmp] = states[id2];
		}
	}
	std::string str;
	while (std::getline(input, str)) {
		text.push_back(str + ' ');
	}

	cur = states[0];
	for (int j = 0; j < text.size(); ++j) {
		bool flag = false;
		bool exit = false;
		for (int i = 0; i < text[j].size(); ++i) {
			/*if (text_queue == "return") {
				int sdfg = 1;
			}*/
			if (text[j][i] == '/' && text[j][i + 1] == '/') {
				prev = cur;
				cur = states[0];
				if (prev != nullptr && prev->is_terminal) {
					if (stuff.find(text_queue) == stuff.end())
						std::cout << result[prev->type] << ' ' << text_queue << "\n";
					else {
						std::cout << 1 << ' ' << text_queue << "\n";
					}
				}
				if (prev != nullptr && prev != states[0] && !prev->is_terminal) {
					std::cout << "ERROR" << ' ' << text_queue << "\n";
					exit = true;
					break;
				}
				text_queue = "";
				prev = nullptr;
				flag = true;
				break;
			}

			if (text[j][i] == ' ' || text[j][i] == '\n' || text[j][i] == '\t' || text[j][i] == '\r') {
				prev = cur;
				cur = states[0];
				if (prev != nullptr && prev->is_terminal) {
					if (stuff.find(text_queue) == stuff.end())
						std::cout << result[prev->type] << ' ' << text_queue << "\n";
					else {
						std::cout << 1 << ' ' << text_queue << "\n";
					}
				}
				if (prev != nullptr && prev != states[0] && !prev->is_terminal) {
					std::cout << "ERROR" << ' ' << text_queue << "\n";
					exit = true;
					break;
				}
				text_queue = "";
				continue;
			}
			else {
				prev = cur;
				cur = cur->trans[text[j][i]];
			}
			if (cur == nullptr) {
				if (prev != nullptr && prev->is_terminal) {
					if (stuff.find(text_queue) == stuff.end())
						std::cout << result[prev->type] << ' ' << text_queue << "\n";
					else {
						std::cout << 1 << ' ' << text_queue << "\n";
					}
				}
				if (prev != nullptr && prev != states[0] && !prev->is_terminal) {
					std::cout << "ERROR" << ' ' << text_queue << "\n";
					exit = true;
					break;
				}

				cur = states[0]->trans[text[j][i]];
				text_queue = "";
				text_queue.push_back(text[j][i]);
			}
			else {
				text_queue.push_back(text[j][i]);
			}
		}
		if (exit) {
			std::cout << "Lex Error";
			return;
		}
		if (flag) {
			flag = false;
			continue;
		}
	}
	if (text_queue == "int" || text_queue == "char" || text_queue == "string" || text_queue == "bool" || text_queue == "float" || text_queue == "void")
		lexems.push_back(Lex(8, text_queue));
	else
		lexems.push_back(Lex(result[cur->type], text_queue));

	if (cur != nullptr && cur->is_terminal) {
		if (text_queue == "int" || text_queue == "char" || text_queue == "string" || text_queue == "bool" || text_queue == "float" || text_queue == "void")
			std::cout << 8 << ' ' << text_queue << "\n";
		else
			std::cout << result[cur->type] << ' ' << text_queue << "\n";
	}
}

Lex Lexical::get_lex() {
	++index;
	if (index >= lexems.size())
		return Lex();
	return lexems[index];
}

void Lexical::low() {
	--index;
	index = std::max(-1, index);
}

int Lexical::get_index() {
	return index;
}

void Lexical::set_index(int ind) {
	index = ind;
}