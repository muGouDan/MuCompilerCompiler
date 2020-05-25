#pragma once
#include "BaseWord.h"
#include <fstream>
#include <vector>
#define CHAR_BUFFER_MAX 256
namespace Scanner
{
	class Identifier :public BaseWord
	{
		const static size_t HEAD = 1;
		char char_buffer[CHAR_BUFFER_MAX] = "\0";
		size_t char_iter = 0;
		std::vector<std::string> keywords;
		std::string str_helper;
		char last_input = '\0';
		void LoadConfig();
	public:
		Identifier() :BaseWord(TokenType::identifier,ConsoleForegroundColor::enmCFC_White)
		{
			LoadConfig();
		}
		virtual bool Scann(char input, const size_t line_no, const size_t iter,char next) override;
	};
}


