#pragma once
#include "BaseWord.h"
#include <vector>
#define CHAR_BUFFER_MAX 256
namespace Scanner
{
	// (1-9)+(0-9)*$
	// (1-9)+(0-9)*.(0-9)*$
	// (0-9).(0-9)*$
	class Digit :public BaseWord
	{
		static const size_t BEGIN_WITH_0 = 1;
		static const size_t NOT_BEGIN_WITH_0 = 2;
		static const size_t AFTER_ONE_POINT = 3;
		static const size_t AFTER_ONE_POINT_HAS_NUM = 4;
		char char_buffer[CHAR_BUFFER_MAX] = "\0";
		size_t char_iter = 0;
		std::vector<std::string> keywords;
		std::string str_helper;
		char last_char = '\0';
	public:
		Digit():BaseWord(TokenType::digit,(ConsoleForegroundColor)6/*yellow*/)
		{}
		virtual bool Scann(char input, const size_t line_no, const size_t iter,char next) override;
	};
}


