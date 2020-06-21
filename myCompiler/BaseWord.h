#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include "Highlight.h"
#define KEYWORD_CFG "my_keyword.cfg"
namespace Scanner
{
	//enum ParseResult
	//{
	//	success = 1,
	//	end_of_line = 1 << 1,
	//	end_of_file = 1 << 2,
	//	error = 1 << 3,
	//};

	enum class TokenType
	{
		none,
		rel_op,
		arith_op,
		log_op,
		number,
		identifier,
		assign,
		keyword,
		separator,
		raw_string,
		custom_type,
		end_symbol
	};
 
	std::string TokenTypeToString(TokenType type);
	TokenType StringToTokenType(std::string str);

	struct Token
	{
		TokenType type = TokenType::none;
		ConsoleForegroundColor color = ConsoleForegroundColor::enmCFC_White;
		std::string name;
		size_t encode = 0;
		size_t line = 0;
		size_t start = 0;
		size_t end = 0;
		size_t length()
		{
			return end - start + 1;
		}
	};

	std::ostream& operator << (std::ostream& out, const Token& token);
	
	class BaseWord
	{
	protected:
		TokenType type = TokenType::none;
		ConsoleForegroundColor color = ConsoleForegroundColor::enmCFC_White;
		static const size_t START = 0;
		Token buffer_token;
		constexpr size_t GetHash(const char* str) const
		{
			if (!*str)
				return 0;
			register size_t hash = 5381;
			while (size_t ch = (size_t)*str++)
			{
				hash += (hash << 5) + ch;
				return hash;
			}
		}
		//swap the buffer, set the state as START and set token_valid true
		virtual void CompleteTokenAndSwap(const char* name, size_t line_no, size_t end)
		{		
			buffer_token.name = name;
			buffer_token.type = type;
			buffer_token.color = color;
			buffer_token.encode = GetHash(name);
			buffer_token.end = end;
			buffer_token.line = line_no;
			auto temp = std::move(buffer_token);
			buffer_token = std::move(current_token);
			current_token = std::move(temp);
			state = START;
			token_valid = true;
		}
	public:
		BaseWord(TokenType type, ConsoleForegroundColor color):
			type(type),color(color)
		{}

		bool token_valid = false;
		Token current_token;
		size_t state = START;
		virtual ~BaseWord() {}
		virtual bool Scann(char input, const size_t line_no, const size_t iter,const char next = '\0') = 0;
		/*****Something May HELP***
		token_valid = false;
		bool roll_back = true;
		while (roll_back)
		{
			  roll_back = false;
		}
		**************************/
	};
}

