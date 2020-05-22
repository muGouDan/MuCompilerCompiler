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
#include "Syntax.h"
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
		std::cout << "[" << std::to_string(i) << "\t]" << input[i] << std::endl;
	}
	std::cout << "Scanner:" << std::endl;
	std::vector<Token> token_set = EasyScanner(input);
	token_set.push_back(Token{ TokenType::end_symbol });
	Highlight(input, token_set);

	//ArithmeticExpression arithmeticExpression;
	//arithmeticExpression.Parse(token_set, 0);

	//for (auto token : token_set)
	//{
	//	std::cout << token << std::endl;
	//}
	
	{	
		SLRParser<Example::LR::symbol> slr(
			Example::LR::pro, Example::LR::id,Example::LR::end,
			Example::LR::epsilon, Example::LR::E_);
		std::cout << "\n\n\nHEAD Parse" << std::endl;
		slr.Parse(token_set, transfer);


		SyntaxLoader syntax("my_syntax.cfg");
		std::cout << "\n\n\nAuto Parse" << std::endl;
		syntax.Parse(token_set);
	}
}

//T(*)(const Scanner::Token&);
Example::LR::symbol transfer(const Scanner::Token& token)
{
	Example::LR::symbol ret = Example::LR::symbol::none;
	switch (token.type)
	{
	case Scanner::TokenType::identifier:
	case Scanner::TokenType::digit:
		ret = Example::LR::symbol::id;
		break;
	case Scanner::TokenType::arith_op:
		if (token.name == "+" || token.name == "-")
			ret = Example::LR::symbol::plus;
		else if (token.name == "*" || token.name == "/")
			ret = Example::LR::symbol::mul;
		else if (token.name == "(")
			ret = Example::LR::symbol::lp;
		else if (token.name == ")")
			ret = Example::LR::symbol::rp;
		break;
	case Scanner::TokenType::end_symbol:
		ret = Example::LR::symbol::end;
		break;
	default:
		ret = Example::LR::symbol::none;
		break;
	}
	return ret;
}