#include "ArithmeticOperator.h"

bool Scanner::ArithmeticOperator::Scann(char input, const size_t line_no, const size_t iter)
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
				buffer_token.start = iter;
				state = MINUS;
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
		case MINUS:
			if (input != '>')
				CompleteTokenAndSwap("-", line_no, iter - 1);
			else
				state = START;
			break;
		default:
			throw(std::logic_error("no such state"));
			break;
		}
	}
	return token_valid;
}
