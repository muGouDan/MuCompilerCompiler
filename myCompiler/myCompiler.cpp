#include <iostream>
#include <fstream>
#include <vector>
#include "Scanner.h"
#include "FileLoader.h"
#include "Highlight.h"
#include "Parser.h"
#include "SetHelper.h"
#include "Examples.h"
using namespace Scanner;
using namespace Parser;
Example::LR::symbol transfer(const Scanner::Token& token);
std::vector<Token> token_set;

int main()
{
	auto input = FileLoader("parser_test.txt");
	//auto input = FileLoader("scanner_test.txt");

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
	Digit digit;
	Assignment assign;
	Separator separator;

	bool omit = false;
	for (size_t line = 0; line < input.size(); ++line)
	{
		auto str = input[line].c_str();
		size_t iter = 0;
		char c;
		bool skip = false;
		do
		{
			c = str[iter];
			if (c == '/')
			{
				// for "//" comment
				if (str[iter + 1] == '/')
				{
					skip = true;
					break;
				}
				// for "/*"
				else if (str[iter + 1] == '*')
				{
					omit = true;
					++iter;
				}
			}
			else if (c == '*' && str[iter + 1] == '/')
			{
				omit = false;
				iter += 2;
				if (str[iter] == '\0')
					break;
			}
			// omit words between /**/
			if (omit)
			{
				++iter;
				continue;
			}
			if (digit.Scann(c, line, iter))
			{
				token_set.push_back(digit.current_token);
			}
			if (assign.Scann(c, line, iter))
			{
				token_set.push_back(assign.current_token);
			}
			if (keyword.Scann(c, line, iter))
			{
				token_set.push_back(keyword.current_token);
			}
			if (identifier.Scann(c, line, iter))
			{
				token_set.push_back(identifier.current_token);
			}
			if (RelOp.Scann(c, line, iter))
			{
				token_set.push_back(RelOp.current_token);
			}
			if (LogOp.Scann(c, line, iter))
			{
				token_set.push_back(LogOp.current_token);
			}
			if (arithOp.Scann(c, line, iter))
			{
				token_set.push_back(arithOp.current_token);
			}
			//last to do: sparator 
			if (separator.Scann(c, line, iter))
			{
				token_set.push_back(separator.current_token);
			}
			++iter;
		} while (c != '\0');

		if (skip)
			continue;
	}

	Highlight(input, token_set);

	//ArithmeticExpression arithmeticExpression;
	//arithmeticExpression.Parse(token_set, 0);

	//for (auto token : token_set)
	//{
	//	std::cout << token << std::endl;
	//}
	

	//LL(1)
	//{
	//	auto first_table = SetHelper<Example::LL1::symbol>::FIRST(
	//		Example::LL1::pro, Example::LL1::epsilon, Example::LL1::id, Example::LL1::E);
	//	std::cout << "\nfirst_table" << std::endl;
	//	//SetHelper<Example::LL1::symbol>::Show(first_table);
	//
	//	auto follow_table = SetHelper<Example::LL1::symbol>::FOLLOW(
	//		first_table, Example::LL1::pro, Example::LL1::epsilon, Example::LL1::end, Example::LL1::E);
	//	std::cout << "\nfollow_table" << std::endl;
	//	SetHelper<Example::LL1::symbol>::Show(follow_table);
	//
	//	auto LL1_table = SetHelper<Example::LL1::symbol>::Preanalysis(
	//		first_table, follow_table, Example::LL1::pro, Example::LL1::epsilon, Example::LL1::end, Example::LL1::E);
	//	std::cout << "\nLL1_table" << std::endl;
	//	SetHelper<Example::LL1::symbol>::Show(LL1_table);
	//}

	//SLR
	//{
	//	auto first_table = SetHelper<Example::LR::symbol>::FIRST(
	//		Example::LR::pro, Example::LR::epsilon, Example::LR::id, Example::LR::E_);

	//	auto follow_table = SetHelper<Example::LR::symbol>::FOLLOW(
	//		first_table, Example::LR::pro, Example::LR::epsilon, Example::LR::end, Example::LR::E_);
	//	std::cout << "\nfollow_table" << std::endl;
	//	SetHelper<Example::LR::symbol>::Show(follow_table);
	//
	//	auto back = SetHelper<Example::LR::symbol>::COLLECTION(
	//		Example::LR::pro, Example::LR::epsilon, Example::LR::end, Example::LR::E_);
	//
	//	const auto& states = std::get<0>(back);
	//	const auto& goto_table = std::get<1>(back);
	//
	//	auto action_table = SetHelper<Example::LR::symbol>::SetActionTable(
	//		Example::LR::pro, states, goto_table, follow_table, 
	//		Example::LR::epsilon, Example::LR::end, Example::LR::E_);
	//}

	{	
		SLRParser<Example::LR::symbol> slr(
			Example::LR::pro, Example::LR::id,Example::LR::end,
			Example::LR::epsilon, Example::LR::E_);
		slr.Parse(token_set, transfer);
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
	case Scanner::TokenType::separator:
		ret = Example::LR::symbol::end;
		break;
	default:
		ret = Example::LR::symbol::end;
		break;
	}
	return ret;
}