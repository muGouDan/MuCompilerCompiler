#pragma once
#include "BaseWord.h"
namespace Scanner
{
	class ArithmeticOperator:public BaseWord
	{
	public:
		ArithmeticOperator() :BaseWord(TokenType::arith_op)
		{}
		virtual bool Scann(char input, const size_t line_no, const size_t iter) override;
	};
}
