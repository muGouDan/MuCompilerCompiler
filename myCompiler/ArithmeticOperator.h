#pragma once
#include "BaseWord.h"
namespace Scanner
{
	class ArithmeticOperator:public BaseWord
	{
		//const static size_t DIV = 1;
		const static size_t MINUS = 1;
	public:
		ArithmeticOperator() :BaseWord(TokenType::arith_op,ConsoleForegroundColor::enmCFC_Red)
		{}
		virtual bool Scann(char input, const size_t line_no, const size_t iter,char next) override;
	};
}
