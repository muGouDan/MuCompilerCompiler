#pragma once
#include "Highlight.h"

void SetConsoleColor(ConsoleForegroundColor foreColor, ConsoleBackGroundColor backColor)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, foreColor | backColor);
}