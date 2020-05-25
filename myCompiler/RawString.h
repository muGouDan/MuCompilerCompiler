#pragma once
#include "BaseWord.h"
namespace Scanner
{
	class RawString :public BaseWord
	{
		const static size_t STR_ON = 1;
		const static size_t CHAR_ON = 2;
		const static size_t CHAR_BODY = 3;
		char buffer[128];
		size_t buffer_iter = 0;
	public:
		RawString() :BaseWord(TokenType::raw_string, (ConsoleForegroundColor)12)
		{
			buffer[0] = '\0';
		}
		virtual bool Scann(char input, const size_t line_no, const size_t iter,char next) override;
		bool YetNotStart()
		{
			return state == START;
		}
	};
}