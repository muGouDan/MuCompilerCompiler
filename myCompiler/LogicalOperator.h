#pragma once
#include "BaseWord.h"
namespace Scanner
{
	// << >> ! && || & | 
	class LogicalOperator :public BaseWord
	{
		const static size_t SINGLE_LESS= 1;
		const static size_t SINGLE_GREATER = 2;
		const static size_t SINGLE_AND = 3;
		const static size_t SINGLE_OR = 4;
	public:
		LogicalOperator() :BaseWord(TokenType::log_op, ConsoleForegroundColor::enmCFC_Purple)
		{}
		virtual bool Scann(char input, const size_t line_no, const size_t iter) override;
	};

}

