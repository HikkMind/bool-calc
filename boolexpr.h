#include <vector>
#include <fstream>
//#include <iostream>

class BooleanExpression
{
	std::vector<std::vector<char>> vars;

	struct Node {
		int var_ind = -1;
		short bool_val = -1;
		unsigned char symbol = '\0';
		Node* left = nullptr;
		Node* right = nullptr;
		Node* parent = nullptr;
	};
	Node* root;

	static bool is_operator(char);
	bool is_linear();
	bool calc(const char*);

public:

	void CNF(std::ofstream&);
	void DNF(std::ofstream&);
	void ZH(std::ofstream&);
	bool* isFull();

	BooleanExpression(const char*);
	~BooleanExpression();

};

