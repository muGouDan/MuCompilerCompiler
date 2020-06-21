#pragma once
#include <string>
#include <vector>
#include <stack>
#include "BaseWord.h"
#define COLUMN (end - elem + 1)
#define ROW (F + 1)
namespace Parser
{
	struct Production//S->AB
	{
		Production()
		{}
		Production(std::initializer_list<int> list) :
			valid(true)
		{
			production = list;
		}
		Production(std::string message, std::initializer_list<int> list) :
			message(message),
			valid(true)
		{
			production = list;
		}
		Production(std::string message):
			message(message),
			valid(false)
		{}
		std::vector<int> production;
		std::string message;
		bool valid = false;
	};

	struct AstNode
	{
		std::unique_ptr<AstNode> first_child;
		std::unique_ptr<AstNode> first_cousion;
	};

	class ArithmeticExpression
	{
	public:
		ArithmeticExpression()
		{
			if(!ready)
				SetUpTable();
		}
		void Parse(std::vector<Scanner::Token>& token_set, size_t start);
	private:
		enum symbol
		{
			none = -1,
			E,
			E_,
			T,
			T_,
			F,
			//---
			elem,
			plus_and_minus,
			mul_and_div,
			left_p,
			right_p,
			end,
			nil
		};	
		std::stack<symbol> parser_stack;
		static constexpr int GetIndex(symbol Sym)
		{
			if (Sym < elem)
				return Sym;
			else
				return Sym - elem;
		}	
		static bool ready;
		static Production table[ROW][COLUMN];
		void SetUpTable();
		symbol GetTerm(Scanner::Token token)
		{
			symbol ret = symbol::none;
			switch (token.type)
			{
			case Scanner::TokenType::identifier:
			case Scanner::TokenType::number:
				ret = elem;
				break;
			case Scanner::TokenType::arith_op:
				if (token.name == "+"||token.name == "-")
					ret = plus_and_minus;
				else if (token.name == "*"||token.name == "/")
					ret = mul_and_div;
				else if (token.name == "(")
					ret = left_p;
				else if (token.name == ")")
					ret = right_p;
				break;
			case Scanner::TokenType::separator:
				ret = end;
				break;
			default:
				break;
			}
			return ret;
		}
	};
	
}

