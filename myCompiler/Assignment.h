#pragma once
#include "BaseWord.h"
namespace Scanner
{
	class Assignment :public BaseWord
	{
		static const size_t EQ = 1;
	public:
		Assignment():BaseWord(TokenType::assign){}
		virtual bool Scann(char input, const size_t line_no, const size_t iter) override;
	};
}


