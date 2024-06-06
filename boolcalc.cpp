#include <fstream>
#include <iostream>
#include "boolexpr.h"
#include <stdexcept>

int main(int argc, char* argv[])
{
	
	std::ifstream input_file(argv[argc - 2], std::ios::in | std::ios::binary);
	std::ofstream output_file(argv[argc - 1], std::ios::out | std::ios::binary);

	char* buffer;

	try{
		if (argv[argc - 3][1] == 'c') {
			bool is_first = true;
			while (!input_file.eof()) {
				buffer = new char[128];
				input_file.getline(buffer, 128);
				if (buffer[0] == '\0') {
					delete[] buffer;
					continue;
				}
				BooleanExpression exp(buffer);
				if (is_first) {
					is_first = false;
				}
				else {
					output_file << std::endl;
				}
				exp.CNF(output_file);
				delete[] buffer;
			}
			
		}
		else if (argv[argc - 3][1] == 'd') {
			bool is_first = true;
			while (!input_file.eof()) {
				buffer = new char[128];
				input_file.getline(buffer, 128);
				if (buffer[0] == '\0') {
					delete[] buffer;
					continue;
				}
				BooleanExpression exp(buffer);
				if (is_first) {
					is_first = false;
				}
				else {
					output_file << std::endl;
				}
				exp.DNF(output_file);
				delete[] buffer;
			}
		}
		else if (argv[argc - 3][1] == 'i') {
			bool* actual;
			bool result[5];
			for (int i = 0; i < 5; ++i) {
				result[i] = false;
			}
			bool is_out = false;
			while (!input_file.eof()) {
				buffer = new char[128];
				input_file.getline(buffer, 128);
				if (buffer[0] == '\0') {
					delete[] buffer;
					continue;
				}
				BooleanExpression exp(buffer);
				actual = exp.isFull();
				for (int i = 0; i < 5; ++i) {
					result[i] = result[i] || actual[i];
				}
				delete[] actual;
				delete[] buffer;
			}
			for (int i = 0; i < 5; ++i) {
				if (!result[i]) {
					output_file << "no";
					is_out = true;
					break;
				}
			}
			if (!is_out) {
				output_file << "yes";
			}
		}
		else if (argv[argc - 3][1] == 'z') {
			bool is_first = true;
			while (!input_file.eof()) {
				buffer = new char[128];
				input_file.getline(buffer, 128);
				if (buffer[0] == '\0') {
					delete[] buffer;
					continue;
				}
				BooleanExpression exp(buffer);
				if (is_first) {
					is_first = false;
				}
				else {
					output_file << std::endl;
				}
				exp.ZH(output_file);
				for (int i = 0; i < 128; ++i) {
					buffer[i] = '\0';
				}
				delete[] buffer;
			}
		}
	} 
	catch(...){
		output_file << "error";
	}

	output_file.close();
	input_file.close();
	return 0;
}
