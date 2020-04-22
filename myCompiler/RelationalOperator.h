#pragma once
#include "BaseWord.h"
namespace Scanner 
{
	// < > <= >= == !=
	class RelationalOperator :public BaseWord
	{
		static const size_t SINGLE_LESS = 1;
		static const size_t SINGLE_GREATER = 2;
		static const size_t SINGLE_EQ = 3;
		static const size_t SINGLE_NE = 4;
		void ResetCurrentToken()
		{
			current_token.name = "None";
			current_token.type = TokenType::none;
			current_token.encode = 0;
			current_token.start = 0;
			current_token.end = 0;
			current_token.line = 0;
		}
	public:
		RelationalOperator() :BaseWord(TokenType::rel_op)
		{}
		//virtual ParseResult Parse(const char* line_src, Token& token, const size_t& line_no, const size_t& start) override;
		virtual bool Scann(char input, const size_t line_no, const size_t iter) override;
	};
}


