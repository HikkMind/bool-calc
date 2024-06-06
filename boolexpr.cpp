#include "boolexpr.h"
#include <stdexcept>
#include <cmath>
#include <iostream>

bool BooleanExpression::is_operator(char c) {
	return c == '+' || c == '^' || c == '~' || c == '&' || c == 'v' || c == '>' || c == '<' || c == '=' || c == '|';
}
bool BooleanExpression::is_linear() {
	int var_count = vars.size();
	int coef_count = static_cast<int>(pow(2, var_count));
	bool* temp_coef = new bool[coef_count];
	char* value_set = new char[var_count];
	//temp_coef[coef_count] = '\0';
	//value_set[var_count] = '\0';

	for (int i = 0; i < var_count; ++i) {
		value_set[i] = '0';
	}

	for (int i = 0; i < coef_count; ++i)
	{
		temp_coef[i] = calc(value_set);

		for (int j = var_count - 1; j >= 0; --j) {
			if (value_set[j] == '1') {
				value_set[j] = '0';
			}
			else {
				value_set[j] = '1';
				break;
			}
		}
	}

	int border = coef_count;
	for (int i = 0; i < coef_count; ++i)
	{
		bool is_one = false;
		if (temp_coef[0] && i != 0) {
			int temp = i;
			for (int j = var_count - 1; j >= 0; --j) {
				if (temp % 2 == 1 && !is_one) {
					is_one = true;
				}
				else if (temp % 2 == 1) {
					delete[] temp_coef;
					delete[] value_set;
					return false;
				}
				temp /= 2;
			}
		}
		int j = 0;
		while (j < border - 1) {
			temp_coef[j] = (temp_coef[j] != temp_coef[j + 1]);
			++j;
		}
		--border;
	}

	delete[] temp_coef;
	delete[] value_set;
	return true;
}
bool BooleanExpression::calc(const char* value_set) {
	if (value_set == nullptr){
		return false;
	}
	Node* actual_node = root;
	while (root->bool_val == -1) {
		while (actual_node->bool_val == -1) {
			if (actual_node->left == nullptr && actual_node->right == nullptr){
				break;
			}
			if(actual_node->symbol == '~' && actual_node->left != nullptr && actual_node->left->bool_val != -1){
				break;
			}
			if (actual_node->symbol != '~' && actual_node->left != nullptr && actual_node->left->bool_val != -1 && actual_node->right->bool_val != -1) {
				break;
			}
			if (actual_node->left != nullptr && actual_node->left->bool_val == -1) {
				actual_node = actual_node->left;
			}
			else if (actual_node->right->bool_val == -1) {
				actual_node = actual_node->right;
			}
		}
		if (is_operator(static_cast<char>(actual_node->symbol))) {
			if (actual_node->symbol == '~' && actual_node->left != nullptr) {
				actual_node->bool_val = static_cast<short>(abs(1 - actual_node->left->bool_val));
				actual_node->left->bool_val = -1;
			}
			else if (actual_node->left != nullptr) {
				switch (actual_node->symbol) {
				case '&':
					actual_node->bool_val = static_cast<short>(actual_node->left->bool_val == 1 && actual_node->right->bool_val == 1);
					break;
				case 'v':
					actual_node->bool_val = static_cast<short>(!(actual_node->left->bool_val == 0 && actual_node->right->bool_val == 0));
					break;
				case '=':
					actual_node->bool_val = static_cast<short>(actual_node->left->bool_val == actual_node->right->bool_val);
					break;
				case '>':
					actual_node->bool_val = static_cast<short>(!(actual_node->left->bool_val == 1 && actual_node->right->bool_val == 0));
					break;
				case '<':
					actual_node->bool_val = static_cast<short>(!(actual_node->right->bool_val == 1 && actual_node->left->bool_val == 0));
					break;
				case '+':
					actual_node->bool_val = static_cast<short>(actual_node->left->bool_val != actual_node->right->bool_val);
					break;
				case '|':
					actual_node->bool_val = static_cast<short>(!(actual_node->left->bool_val == 1 && actual_node->right->bool_val == 1));
					break;
				case '^':
					actual_node->bool_val = static_cast<short>(actual_node->left->bool_val == 0 && actual_node->right->bool_val == 0);
					break;
				}

				
				if (actual_node->left->symbol != '1' && actual_node->left->symbol != '0') {
					actual_node->left->bool_val = -1;
				}
				else {
					actual_node->left->bool_val = static_cast<short>(actual_node->left->symbol == '1');
				}
				if (actual_node->right->symbol != '1' && actual_node->right->symbol != '0') {
					actual_node->right->bool_val = -1;
				}
				else {
					actual_node->right->bool_val = static_cast<short>(actual_node->right->symbol == '1');
				}
			}
		}
		else if (actual_node->symbol != '1' && actual_node->symbol != '0') {
			actual_node->bool_val = static_cast<short>(value_set[actual_node->var_ind] == '1');
		}
		actual_node = actual_node->parent;
	}

	bool result = (root->bool_val == 1);
	if (root->symbol != '1' && root->symbol != '0') {
		root->bool_val = -1;
	}
	else {
		root->bool_val = static_cast<short>(root->symbol == '1');
	}

	return result;
}


void BooleanExpression::CNF(std::ofstream& output_file) {

	int var_count = vars.size();
	char* value_set = new char[var_count + 1];
	value_set[var_count] = '\0';
	for (int i = 0; i < var_count; ++i) {
		value_set[i] = '0';
	}

	if (var_count == 0){
		bool result = calc(value_set);
		if (result){
			output_file << '1';
		}
		else {
			output_file << '0';
		}
		delete[] value_set;
		return;
	}

	bool process = true;
	bool is_fir_con = true;
	while (process) {
		if (!calc(value_set)) {
			if (!is_fir_con) {
				output_file << " & ";
			}
			else {
				is_fir_con = false;
			}
			output_file << '(';
			bool is_first = true;
			for (int i = 0; i < var_count; ++i) {
				if (!is_first) {
					output_file << 'v';
				}
				else
				{
					is_first = false;
				}
				if (value_set[i] == '1') {
					output_file << '~';
				}
				for (char j : vars[i]) {
					output_file << j;
				}
				
			}
			output_file << ')';
		}

		for (int j = var_count - 1; j >= 0; --j) {
			if (value_set[j] == '1') {
				value_set[j] = '0';
			}
			else {
				value_set[j] = '1';
				break;
			}
			if (j == 0) {
				process = false;
				break;
			}
		}
	}


	delete[] value_set;
}
void BooleanExpression::DNF(std::ofstream& output_file) {
	int var_count = vars.size();
	char* value_set = new char[var_count + 1];
	value_set[var_count] = '\0';
	for (int i = 0; i < var_count; ++i) {
		value_set[i] = '0';
	}

	if (root->bool_val != -1) {
		output_file << root->bool_val;
		delete[] value_set;
		return;
	}


	bool process = true;
	bool is_fir_con = true;
	while (process) {
		if (calc(value_set)) {
			if (!is_fir_con) {
				output_file << " v ";
			}
			else {
				is_fir_con = false;
			}
			output_file << '(';
			bool is_first = true;
			for (int i = 0; i < var_count; ++i) {
				if (!is_first) {
					output_file << '&';
				}
				else
				{
					is_first = false;
				}
				if (value_set[i] == '0') {
					output_file << '~';
				}
				for (char j : vars[i]) {
					output_file << j;
				}

			}
			output_file << ')';
		}

		for (int j = var_count - 1; j >= 0; --j) {
			if (value_set[j] == '1') {
				value_set[j] = '0';
			}
			else {
				value_set[j] = '1';
				break;
			}
			if (j == 0) {
				process = false;
				break;
			}
		}
	}


	delete[] value_set;
}
void BooleanExpression::ZH(std::ofstream& output_file) {

	int var_count = vars.size();
	int coef_count = static_cast<int>(pow(2, var_count));

	bool* temp_coef = new bool[coef_count + 1];
	char* value_set = new char[var_count + 1];
	value_set[var_count] = '\0';

	for (int i = 0; i < var_count; ++i) {
		value_set[i] = '0';
	}

	bool process = true;
	for (int i = 0; i < coef_count; ++i)
	{
		temp_coef[i] = calc(value_set);

		for (int j = var_count - 1; j >= 0; --j) {
			if (value_set[j] == '1') {
				value_set[j] = '0';
			}
			else {
				value_set[j] = '1';
				break;
			}
		}
	}

	bool is_first = true;
	bool is_cnst = true;
	int border = coef_count;
	for (int i = 0; i < coef_count; ++i) 
	{
		if (temp_coef[0] && i != 0) {
			is_cnst = false;
			int temp = i;
			for (int j = var_count-1; j >=0 ; --j) {
				value_set[j] = temp % 2 == 1 ? '1' : '0';
				temp /= 2;
			} 
			if (!is_first) {
				output_file << " + ";
			}
			else {
				is_first = false;
			}
			bool is_first_var = true;
			for (int j = 0; j < var_count; ++j) {
				if (value_set[j] == '1') {
					if (!is_first_var) {
						output_file << '&';
					}
					else {
						is_first_var = false;
					}
					for (char x : vars[j]) {
						output_file << x;
					}
				}
			}
		}
		else if (temp_coef[0]) {
			output_file << '1';
			is_first = false;
			is_cnst = false;
		}
		int j = 0;
		while (j < border - 1) {
			temp_coef[j] = (temp_coef[j] != temp_coef[j + 1]);
			++j;
		}
		--border;
	}

	if (is_cnst){
		output_file << '0';
	}
	delete[] temp_coef;
	delete[] value_set;
}
bool* BooleanExpression::isFull() {
	bool* fun_charc = new bool[6]; // 0-не сохраняет 0, 1-не сохраняет 1, 2-несамодвойственность, 3-немонотонность, 4-нелинейность
	for (int i = 0; i < 5; ++i) {
		fun_charc[i] = false;
	}

	int var_count = vars.size();
	char* value_set = new char[var_count + 1];
	value_set[var_count] = '\0';
	for (int i = 0; i < var_count; ++i) {
		value_set[i] = '1';
	}
	fun_charc[1] = !calc(value_set);
	
	for (int i = 0; i < var_count; ++i) {
		value_set[i] = '0';
	}
	char* t_value_set = new char[var_count + 1];
	char* m_value_set = new char[var_count + 1];
	t_value_set[var_count] = '\0';
	m_value_set[var_count] = '\0';
	for (int i = 0; i < var_count; ++i) {
		t_value_set[i] = '1';
		m_value_set[i] = '1';
	}
	fun_charc[0] = calc(value_set);

	if (var_count == 0) {
		fun_charc[2] = true;
		fun_charc[3] = true;
		fun_charc[4] = false;
		delete[] value_set;
		delete[] t_value_set;
		delete[] m_value_set;
		return fun_charc;
	}
	
	bool process = true;
	bool is_min_found = false;

	while (process) 
	{
		bool result = calc(value_set);
		if (result == calc(t_value_set)) {
			fun_charc[2] = true;
		}
		if (result && !is_min_found) {
			is_min_found = true;
			for (int i = 0; i < var_count; ++i) {
				m_value_set[i] = value_set[i];
			}
		}
		else if (!result && !fun_charc[3]) {
			for (int i = 0; i < var_count; ++i) {
				if (m_value_set[i] < value_set[i]) {
					fun_charc[3] = true;
					break;
				}
			}
		}


		for (int j = var_count - 1; j >= 0; --j) {
			if (value_set[j] == '1') {
				value_set[j] = '0';
				t_value_set[j] = '1';
			}
			else {
				value_set[j] = '1';
				t_value_set[j] = '0';
				break;
			}
			if (j == 0) {
				process = false;
				break;
			}
		}
	}

	fun_charc[4] = !is_linear();
	delete[] t_value_set;
	delete[] m_value_set;
	delete[] value_set;
	return fun_charc;
}


BooleanExpression::BooleanExpression(const char* input)
{
	int brecket_count = 0;
	int ind = 0;
	int var_ind = 0;
	bool is_right_operand = false;
	bool is_var = false;
	while (input[ind] != '\0') 
	{
		if ((input[ind] > '9' || input[ind] < '0') && input[ind] != 'x' && input[ind] != '(' && input[ind] != ')'
			&& input[ind] != ' ' && input[ind] != '~' && input[ind] != '&' && input[ind] != 'v'
			&& input[ind] != '+' && input[ind] != '>' && input[ind] != '<' && input[ind] != '='
			&& input[ind] != '|' && input[ind] != '^' && input[ind] != '\r' && input[ind] != '\n') {
			throw std::invalid_argument(input);
		}
		++ind;
	}
	
	ind = 0;
	while (input[ind] != '\0')
	{
		if (input[ind] == '(') {
			is_var = false;
			++brecket_count;
		}
		else if (input[ind] == ')') {
			--brecket_count;
			if (!is_var) {
				break;
			}
			is_var = true;
			if (brecket_count < 0) {
				break;
			}
		}
		else if (input[ind] == 'x' || input[ind] <= '9' && input[ind] >= '0') {
			if (is_var) {
				break;
			}
			is_var = true;
			++ind;
			while (input[ind] <= '9' && input[ind] >= '0') {
				++ind;
			}
			--ind;
		}
		else if (input[ind] == '+' || input[ind] == '=' || input[ind] == '&' || input[ind] == 'v' 
			|| input[ind] == '>' || input[ind] == '<' || input[ind] == '|' || input[ind] == '^') {
			if (!is_var) {
				break;
			}
			is_var = false;
		}
	
		if (input[ind] != '\0') {
			++ind;
		}
	}
	
	if (brecket_count != 0 || input[ind] != '\0') {
		throw std::invalid_argument(input);
	}
	
	Node* actual_node = new Node;
	int t_var_ind = -1;
	for (int ind = 0; input[ind] != '\0'; ++ind) {

		if (input[ind] == 'x') {
			std::vector<char> t_var(1);
			t_var[0] = 'x';
			++ind;
			while (input[ind] <= '9' && input[ind] >= '0') {
				t_var.push_back(input[ind]);
				++ind;
			}
			bool is_exist = false;
			for (int i = 0; i < vars.size(); ++i) {
				is_exist = false;
				if (vars[i].size() == t_var.size()) {
					is_exist = true;
					for (int j = 0; j < t_var.size(); ++j)
					{
						if (vars[i][j] != t_var[j]) {
							is_exist = false;
							break;
						}
					}
				}
				if (is_exist) {
					t_var_ind = i;
					break;
				}
			}

			if (!is_exist)
			{
				vars.push_back(t_var);
				t_var_ind = static_cast<int>(vars.size() - 1);
			}



			if (actual_node != nullptr && actual_node->left == nullptr) {
				actual_node->left = new Node;
				actual_node->left->parent = actual_node;
				actual_node->left->var_ind = t_var_ind;
			}
			else if (actual_node != nullptr) {
				actual_node->right = new Node;
				actual_node->right->parent = actual_node;
				actual_node->right->var_ind = t_var_ind;
			}
		}
		else if ((input[ind] == '1' || input[ind] == '0') && actual_node != nullptr) {
			if (ind > 0 && input[ind - 1] != 'x' || (input[ind + 1] < '0' || input[ind + 1] > '9')) {
				if (actual_node->left == nullptr) {
					actual_node->left = new Node;
					actual_node->left->parent = actual_node;
					actual_node->left->symbol = input[ind];
					actual_node->left->bool_val = static_cast<short>(input[ind] == '1');
				}
				else {
					actual_node->right = new Node;
					actual_node->right->parent = actual_node;
					actual_node->right->symbol = input[ind];
					actual_node->right->bool_val = static_cast<short>(input[ind] == '1');
				}
			}
		}

		if (actual_node == nullptr){
			continue;
		}
		switch (input[ind]) {
		case '(':
			actual_node->bool_val *= 2;
			if (actual_node->bool_val == 0) {
				actual_node->bool_val = -3;
			}
			if (actual_node->left == nullptr) {
				actual_node->left = new Node;
				actual_node->left->parent = actual_node;
				actual_node = actual_node->left;
			}
			else if (actual_node->right == nullptr) {
				actual_node->right = new Node;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
				
			break;
		case ')':
			while (actual_node != nullptr && actual_node->bool_val != -2 && actual_node->bool_val != 2 && actual_node->bool_val != -3) {
				actual_node = actual_node->parent;
			}
			if (actual_node != nullptr && actual_node->bool_val == -3) {
				actual_node->bool_val = 0;
			}
			else if (actual_node != nullptr) {
				actual_node->bool_val /= 2;
			}
			break;
		case '~':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '~';
			}
			else if (actual_node->right != nullptr) {
				Node* temp = actual_node->left;
				actual_node->right = new Node;
				actual_node->right->symbol = '~';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				temp->parent = actual_node;
			}
			else{
				actual_node->right = new Node;
				actual_node->right->symbol = '~';
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
			}
			
			break;
		case '&':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '&';
			}
			else if (actual_node->symbol == '~') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '&';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '&';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '&';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '&';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr){
					temp->parent = actual_node;
				}
			}
			break;
		case 'v':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = 'v';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = 'v';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = 'v';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = 'v';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = 'v';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr){
					temp->parent = actual_node;
				}
			}
			break;
		case '=':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '=';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '=';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '=';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '=';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '=';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
			break;
		case '>':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '>';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v' 
				|| actual_node->symbol == '=') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
					|| actual_node->symbol == '=') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '>';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '>';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '>';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '>';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
			break;
		case '<':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '<';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v' 
				|| actual_node->symbol == '=' || actual_node->symbol == '<') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
					|| actual_node->symbol == '=' || actual_node->symbol == '<') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '<';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '<';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '<';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '<';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
			break;
		case '+':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '+';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
				|| actual_node->symbol == '=' || actual_node->symbol == '<' || actual_node->symbol == '>') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
					|| actual_node->symbol == '=' || actual_node->symbol == '<' || actual_node->symbol == '>') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '+';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '+';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '+';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '+';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
			break;
		case '|':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '|';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
				|| actual_node->symbol == '=' || actual_node->symbol == '<' || actual_node->symbol == '>'
				|| actual_node->symbol == '+') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
					|| actual_node->symbol == '=' || actual_node->symbol == '<' || actual_node->symbol == '>'
					|| actual_node->symbol == '+') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '|';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '|';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp; 
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '|';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '|';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
			break;
		case '^':
			if (actual_node->symbol == '\0') {
				actual_node->symbol = '^';
			}
			else if (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
				|| actual_node->symbol == '=' || actual_node->symbol == '<' || actual_node->symbol == '>'
				|| actual_node->symbol == '+' || actual_node->symbol == '|') {
				Node* temp = nullptr;
				while (actual_node->symbol == '~' || actual_node->symbol == '&' || actual_node->symbol == 'v'
					|| actual_node->symbol == '=' || actual_node->symbol == '<' || actual_node->symbol == '>'
					|| actual_node->symbol == '+' || actual_node->symbol == '|') {
					temp = actual_node;
					actual_node = actual_node->parent;
					if (actual_node == nullptr || actual_node->bool_val == -2 || actual_node->bool_val == 2 || actual_node->bool_val == -3) {
						break;
					}
				}
				if (actual_node == nullptr) {
					actual_node = new Node;
					actual_node->left = temp;
					actual_node->symbol = '^';
					temp->parent = actual_node;
				}
				else if (temp == actual_node->left) {
					actual_node->left = new Node;
					actual_node->left->symbol = '^';
					actual_node->left->parent = actual_node;
					actual_node->left->left = temp;
					actual_node = actual_node->left;
					temp->parent = actual_node;
				}
				else {
					actual_node->right = new Node;
					actual_node->right->symbol = '^';
					actual_node->right->parent = actual_node;
					actual_node->right->left = temp;
					actual_node = actual_node->right;
					temp->parent = actual_node;
				}
			}
			else {
				Node* temp = actual_node->right;
				actual_node->right = new Node;
				actual_node->right->symbol = '^';
				actual_node->right->left = temp;
				actual_node->right->parent = actual_node;
				actual_node = actual_node->right;
				if (temp != nullptr) {
					temp->parent = actual_node;
				}
			}
			break;
			
		}
	}
	
	while (actual_node != nullptr && actual_node->parent != nullptr) {
		actual_node = actual_node->parent;
	}

	root = actual_node;
	while (actual_node != nullptr && actual_node->symbol == '\0' && actual_node->bool_val == -1 && actual_node->var_ind == -1) {
		if (actual_node->left != nullptr) {
			root = actual_node->left;
			root->parent = nullptr;
			delete actual_node;
		}
		else if (actual_node->right != nullptr) {
			root = actual_node->right;
			root->parent = nullptr;
			delete actual_node;
		}
		actual_node = root;
	}

	
}
BooleanExpression::~BooleanExpression() {
	Node* actual;
	while (true)
	{
		actual = root;
		Node* temp = nullptr;
		while (actual->left != nullptr || actual->right != nullptr) {
			temp = actual;
			if (actual->left != nullptr) {
				actual = actual->left;
			}
			else {
				actual = actual->right;
			}
		}
	
		if (temp == nullptr) {
			delete actual;
			break;
		}
		if (temp->left == actual) {
			temp->left = nullptr;
		}
		else {
			temp->right = nullptr;
		}
		delete actual;
	}
}
