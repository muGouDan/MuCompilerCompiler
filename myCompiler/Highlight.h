#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
//   0 = black			8 = gray
//   1 = blue			9 = tint blue
//   2 = green			A = tint green
//   3 = light green	B = tint light green
//   4 = red			C = tint red
//   5 = purple			D = tint purple
//   6 = yellow			E = tint yellow
//   7 = white			F = bright white

enum ConsoleBackGroundColor
{
	enmCBC_Red = BACKGROUND_INTENSITY | BACKGROUND_RED,
	enmCBC_Green = BACKGROUND_INTENSITY | BACKGROUND_GREEN,
	enmCBC_Blue = BACKGROUND_INTENSITY | BACKGROUND_BLUE,
	enmCBC_Yellow = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN,
	enmCBC_Purple = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE,
	enmCBC_Cyan = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_HighWhite = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
	enmCBC_Black = 0,
};

enum ConsoleForegroundColor
{
	enmCFC_Red = FOREGROUND_INTENSITY | FOREGROUND_RED,
	enmCFC_Green = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	enmCFC_Blue = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	enmCFC_Yellow = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
	enmCFC_Purple = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	enmCFC_Cyan = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Gray = FOREGROUND_INTENSITY,
	enmCFC_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_HighWhite = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	enmCFC_Black = 0,
};

void SetConsoleColor(ConsoleForegroundColor foreColor = enmCFC_White, ConsoleBackGroundColor backColor = enmCBC_Black);

template<typename Input,typename TokenSet>
void Highlight(Input&& input, TokenSet&& token_set)
{
	size_t start = 0;
	if (input.size())
		start = input[0].line_no;
	size_t token_iter = 0;
	for (size_t i = 0; i < input.size(); i++)
	{
		SetConsoleColor(enmCFC_White);
		std::cout << "[" << input[i].line_no << "\t]";
		size_t j = 0;
		bool over = false;
		while (j < input[i].content.size())
		{
			if (token_iter >= token_set.size())
				over = true;
			if (!over)
			{		
				if (token_set[token_iter].line - start == i
					&& j <= token_set[token_iter].end
					&& token_set[token_iter].start <= j)
				{
					SetConsoleColor(token_set[token_iter].color);
					std::cout << input[i].content[j];
					++j;
				}
				else if (i == token_set[token_iter].line -start && j < token_set[token_iter].start
					|| i < token_set[token_iter].line - start)
				{
					SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
					std::cout << input[i].content[j];
					++j;
				}
				else if (i == token_set[token_iter].line - start && j > token_set[token_iter].end
					|| i > token_set[token_iter].line - start)
				{
					++token_iter;
				}
			}
			else
			{
				SetConsoleColor(enmCFC_Green);
				std::cout << input[i].content[j];
				++j;
			}
		}
		std::cout << std::endl;
	}
	SetConsoleColor(enmCFC_White);
}

template<typename Input, typename TokenSet>
void Highlight(Input&& input, TokenSet&& token_set, size_t error_iter)
{
	auto error = false;
	size_t start = 0;
	if (input.size())
		start = input[0].line_no;
	size_t token_iter = 0;
	for (size_t i = 0; i < input.size(); i++)
	{
		SetConsoleColor(enmCFC_White);
		std::cout << "[" << input[i].line_no << "\t]";
		size_t j = 0;
		bool over = false;
		while (j < input[i].content.size())
		{
			if (token_iter >= token_set.size())
				over = true;
			if (!over)
			{
				if (token_set[token_iter].line - start == i
					&& j <= token_set[token_iter].end
					&& token_set[token_iter].start <= j)
				{
					SetConsoleColor(token_set[token_iter].color);
					std::cout << input[i].content[j];
					++j;
				}
				else if (i == token_set[token_iter].line - start && j < token_set[token_iter].start
					|| i < token_set[token_iter].line - start)
				{
					SetConsoleColor((ConsoleForegroundColor)0x2/*light green*/);
					std::cout << input[i].content[j];					
					++j;
				}
				else if (i == token_set[token_iter].line - start && j > token_set[token_iter].end
					|| i > token_set[token_iter].line - start)
				{
					++token_iter;
					if (token_iter == error_iter) error = true;
				}
			}
			else
			{
				SetConsoleColor(enmCFC_Green);
				std::cout << input[i].content[j];
				++j;
			}
		}
		std::cout << std::endl;
		if (error)
		{
			SetConsoleColor(ConsoleForegroundColor::enmCFC_Red);
			std::cout << "[" << input[i].line_no << "\t]";
			for (size_t t = 0; t < token_set[error_iter].start; ++t)
				std::cout << " ";
			std::cout << "^ERROR Token" << std::endl;
			SetConsoleColor(enmCFC_White);
			error = false;
		}
	}
	SetConsoleColor(enmCFC_White);
}