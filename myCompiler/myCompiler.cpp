#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner.h"
#include "FileLoader.h"
#include "Highlight.h"
#include "Parser.h"
#include "SetHelper.h"
#include "Examples.h"
#include "SyntaxDirected.h"
#include "CalculatorCompiler.h"
#include "ExampleCompiler.h"
#include "ILGenerator.h"
using namespace Scanner;

int main()
{
	auto input = FileLoader("definition_test.txt");
	auto token_set = EasyScanner(input);
	std::cout << "Scanner:" << std::endl;
	Highlight(input, token_set);
	std::cout << "\n" << std::endl;
	ILGenerator iLgenerator("complete_syntax.syn");
	iLgenerator.SetInput(input);
	if (iLgenerator.Parse(token_set))
	{
		Highlight(input, token_set);
		iLgenerator.ShowTables();
	}
}