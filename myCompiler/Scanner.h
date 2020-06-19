#pragma once
#include <string>
#include <iostream>
#include "BaseWord.h"
#include "RelationalOperator.h"
#include "LogicalOperator.h"
#include "ArithmeticOperator.h"
#include "Keyword.h"
#include "Identifier.h"
#include "Digit.h"
#include "Assignment.h"
#include "Separator.h"
#include "RawString.h"
#include "FileLoader.h"
std::vector<Scanner::Token> EasyScanner(const std::vector<LineContent>& input_text);