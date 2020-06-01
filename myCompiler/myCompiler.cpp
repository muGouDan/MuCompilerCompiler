#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.h"
#include "FileLoader.h"
#include "Highlight.h"
#include "Parser.h"
#include "SetHelper.h"
#include "Examples.h"
#include "SyntaxDirected.h"
#include "CalculatorCompiler.h"
#include "ExampleCompiler.h"
#include "ILGenerator.h"
using namespace Scanner;
using namespace Parser;
Example::LR::symbol transfer(const Scanner::Token& token);

int main()
{
	
	auto input = FileLoader("parser_test.txt");
	//auto input = FileLoader("my_syntax.cfg");
	//auto input = FileLoader("scanner_test.txt");
	std::cout << "Input:" << std::endl;
	for (size_t i = 0; i < input.size(); i++)
	{
		std::cout << "[" << std::to_string(input[i].line_no) << "\t]" << input[i].content << std::endl;
	}
	std::cout << "Scanner:" << std::endl;
	std::vector<Token> token_set = EasyScanner(input);
	token_set.push_back(Token{ TokenType::end_symbol });
	Highlight(input, token_set);

	//CalculatorCompiler myCompiler("my_syntax.syn");
	//myCompiler.Parse(token_set);

	auto input_def = FileLoader("definition_test.txt");
	auto def_token_set = EasyScanner(input_def);
	def_token_set.push_back(Token{ TokenType::end_symbol });
	std::cout << "\n" << std::endl;
	ILGenerator iLgenerator("complete_syntax.syn");
	iLgenerator.Parse(def_token_set);
	iLgenerator.ShowTables();

	//TestCompiler tCompiler("test_syn.txt");
}