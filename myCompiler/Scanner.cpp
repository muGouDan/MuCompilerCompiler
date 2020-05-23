#include "Scanner.h"

std::vector<Scanner::Token> EasyScanner(const std::vector<std::string>& input)
{
	std::vector<Scanner::Token> token_set;
	Scanner::RelationalOperator RelOp;
	Scanner::LogicalOperator LogOp;
	Scanner::ArithmeticOperator arithOp;
	Scanner::Keyword keyword;
	Scanner::Identifier identifier;
	Scanner::Digit digit;
	Scanner::Assignment assign;
	Scanner::Separator separator;
	Scanner::RawString raw_string;
	bool omit = false;
	for (size_t line = 0; line < input.size(); ++line)
	{
		auto str = input[line].c_str();
		size_t iter = 0;
		char c;
		bool skip = false;
		do
		{
			c = str[iter];
			if (c == '/')
			{
				// for "//" comment
				if (str[iter + 1] == '/')
				{
					skip = true;
					break;
				}
				// for "/*"
				else if (str[iter + 1] == '*')
				{
					omit = true;
					++iter;
				}
			}
			else if (c == '*' && str[iter + 1] == '/')
			{
				omit = false;
				iter += 2;
				if (str[iter] == '\0')
					break;
			}
			// omit words between /**/
			if (omit)
			{
				++iter;
				continue;
			}
			if (raw_string.Scann(c, line, iter))
			{
				token_set.push_back(raw_string.current_token);
			}
			if (raw_string.YetNotStart())
			{
				if (digit.Scann(c, line, iter))
				{
					token_set.push_back(digit.current_token);
				}
				if (assign.Scann(c, line, iter))
				{
					token_set.push_back(assign.current_token);
				}
				if (keyword.Scann(c, line, iter))
				{
					token_set.push_back(keyword.current_token);
				}
				if (identifier.Scann(c, line, iter))
				{
					token_set.push_back(identifier.current_token);
				}
				if (RelOp.Scann(c, line, iter))
				{
					token_set.push_back(RelOp.current_token);
				}
				if (LogOp.Scann(c, line, iter))
				{
					token_set.push_back(LogOp.current_token);
				}
				if (arithOp.Scann(c, line, iter))
				{
					token_set.push_back(arithOp.current_token);
				}
				//last to do: sparator 
				if (separator.Scann(c, line, iter))
				{
					token_set.push_back(separator.current_token);
				}
			}
			++iter;
		} while (c != '\0');

		if (skip)
			continue;
	}
	return token_set;
}