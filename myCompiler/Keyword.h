#pragma once
#include <vector>
#include <tuple>
#include <fstream>
#include "BaseWord.h"
namespace Scanner
{
	class Keyword :public BaseWord
	{
		using ScannState = size_t;
		using Index = size_t;
		using Pair = std::tuple<std::string, ScannState, Index>;
		std::vector<Pair> pairs;
		std::string str_helper;
		size_t current_index;
		void LoadConfig();
		void RefleshPairs()
		{
			for (auto& pair : pairs)
			{
				std::get<1>(pair) = 0;
				std::get<2>(pair) = 0;
			}
			finished_amount = 0;
			raw_valid = false;
		}
		const static size_t ON = 1;
		const static size_t OFF = 2;
		bool raw_valid = false;
		size_t finished_amount = 0;
	public:
		Keyword():BaseWord(TokenType::keyword,ConsoleForegroundColor::enmCFC_Blue)
		{
			LoadConfig();
		}
		virtual bool Scann(char input, const size_t line_no, const size_t iter) override;
		void SetBufferToken(const char* name, size_t line_no, size_t end)
		{
			buffer_token.name = name;
			buffer_token.type = type;
			buffer_token.color = color;
			buffer_token.encode = GetHash(name);
			buffer_token.end = end;
			buffer_token.line = line_no;
		}
	};
}

