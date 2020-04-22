#include <iostream>
#include <fstream>
#include <vector>
#include "Scanner.h"
#include "FileLoader.h"
using namespace Scanner;
std::vector<Token> token_set;

int main()
{
	auto input = FileLoader("test.txt");
	
	std::cout << "Input:" << std::endl;
	for (size_t i = 0; i < input.size(); i++)
	{
		std::cout << "[" << std::to_string(i) << "\t]" << input[i] << std::endl;
	}
	std::cout << "Scanner:" << std::endl;
	RelationalOperator RelOp;
	LogicalOperator LogOp;
	ArithmeticOperator arithOp;
	Keyword keyword;
	Identifier identifier;
	for (size_t line = 0;line<input.size();++line)
	{
		auto str = input[line].c_str();
		size_t iter = 0;
		char c;
		do
		{
			c = str[iter++];
			//don't omit space
			if (keyword.Scann(c, line, iter))
			{
				token_set.push_back(keyword.current_token);
			}
			if (identifier.Scann(c, line, iter))
			{
				token_set.push_back(identifier.current_token);
			}
			// omit space
			if (c == ' ' || c == '\t') continue;		
			//************************
			if (RelOp.Scann(c, line, iter))
			{
				//std::cout << RelOp.current_token << std::endl;
				token_set.push_back(RelOp.current_token);
			}
			if (LogOp.Scann(c, line, iter))
			{
				//std::cout << LogOp.current_token << std::endl;
				token_set.push_back(LogOp.current_token);
			}
			if (arithOp.Scann(c, line, iter))
			{
				token_set.push_back(arithOp.current_token);
			}
			//************************

		} while (c != '\0');
	}

	for (auto token : token_set)
	{
		std::cout << token << std::endl;
	}
}

