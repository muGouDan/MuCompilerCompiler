#include "Scanner.h"

std::vector<Scanner::Token> EasyScanner(const std::vector<LineContent>& input_text)
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
	for (size_t line = 0; line < input_text.size(); ++line)
	{
		auto str = input_text[line].content.c_str();
		size_t iter = 0;
		char c;
		bool skip = false;
		do
		{
			c = str[iter];
			auto next = c == '\0' ? '\0' : str[iter + 1];
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
				else
					continue;
			}
			// omit words between /**/
			if (omit)
			{
				++iter;
				continue;
			}
			if (raw_string.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(raw_string.current_token);
			if (!raw_string.YetNotStart()) c = '\0';
			if (digit.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(digit.current_token);
			if (assign.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(assign.current_token);
			if (keyword.Scann(c, input_text[line].line_no, iter, next))
			{
				//before Keyword shouldn't be [a...z][A...Z]["_"]
				int before = iter - keyword.current_token.length() -1;				
				if (before < 0 || !(isalpha(str[before]) || str[before] == '_'))
					token_set.push_back(keyword.current_token);
			}
			if (identifier.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(identifier.current_token);
			if (RelOp.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(RelOp.current_token);
			if (LogOp.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(LogOp.current_token);
			if (arithOp.Scann(c, input_text[line].line_no, iter, next))token_set.push_back(arithOp.current_token);
			// last to do: sparator 
			if (separator.Scann(c, input_text[line].line_no, iter, next)) token_set.push_back(separator.current_token);
			// recover the char.
			c = str[iter++];
		} while (c != '\0');

		if (skip)
			continue;
	}
	token_set.push_back(Scanner::Token{ Scanner::TokenType::end_symbol });
	return token_set;
}