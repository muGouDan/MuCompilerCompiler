#pragma once
#include "BaseWord.h"
namespace Scanner
{
	class Separator :public BaseWord
	{
		const static size_t SINGLE_COLON = 1;
		const static size_t SINGLE_MINUS = 2;
		const static size_t SINGLE_POINT = 3;
	public:
		Separator() :BaseWord(TokenType::separator, (ConsoleForegroundColor)14/*tint red*/)
		{}
		virtual bool Scann(char input, const size_t line_no, const size_t iter,char next) override;
	};
}

