#include "ArithmeticOperator.h"

bool Scanner::ArithmeticOperator::Scann(char input, const size_t line_no, const size_t iter,char next)
{
	token_valid = false;
	bool roll_back = true;
	while (roll_back)
	{
		roll_back = false;
		switch (state)
		{
		case START:
			switch (input)
			{
			case '+':
				buffer_token.start = iter;
				CompleteTokenAndSwap("+", line_no, iter);
				break;
			case '-':
				if (next != '>')
				{
					buffer_token.start = iter;
					CompleteTokenAndSwap("-", line_no, iter);
				}
				else
				{
					state = START;
				}
				break;
			case '*':
				buffer_token.start = iter;
				CompleteTokenAndSwap("*", line_no, iter);
				break;
			case '/':
				buffer_token.start = iter;
				CompleteTokenAndSwap("/", line_no, iter);
				break;
			case '(':
				buffer_token.start = iter;
				CompleteTokenAndSwap("(", line_no, iter);
				break;
			case ')':
				buffer_token.start = iter;
				CompleteTokenAndSwap(")", line_no, iter);
				break;
			default:
				state = START;
				break;
			}
			break;
		default:
			throw(std::logic_error("no such state"));
			break;
		}
	}
	return token_valid;
}
