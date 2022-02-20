#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <string.h>
#include <map>
#include <set>
#include <fstream>


std::vector<std::string> text;

struct State {
	std::vector<State*> trans;
	bool is_terminal = false;
	std::string type;
};

struct Lex {
	Lex(int type, std::string val) : type(type), val(val) {}
	Lex() {}
	int type;
	std::string val;
};

std::set<std::string> stuff;
std::set<std::string> operators;
std::set<std::string> separators;
std::map<std::string, int> result;
std::vector<State*> states;
State* cur;
State* prev;
std::string text_queue = "";



class Lexical {
public:
	Lexical();
	Lex get_lex();
private:
	int index = -1;
	std::vector<Lex> lexems;
};

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
	lexems.push_back(Lex(result[cur->type], text_queue));

	if (cur != nullptr && cur->is_terminal) {
		std::cout << result[cur->type] << ' ' << text_queue << "\n";
	}
}

Lex Lexical::get_lex() {
	++index;
	return lexems[index];
}



class Syntactical {
public:
	Syntactical();

	
private:
	Lex lexem;
	int t = lexem.type;
	std::string c = lexem.val;
};

/*void Syntactical::Program() {
	if (t == )
}*/



int main() {
	Lexical* lex = new Lexical();

	return 0;
}