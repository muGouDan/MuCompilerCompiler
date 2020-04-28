#include "Assignment.h"

bool Scanner::Assignment::Scann(char input, const size_t line_no, const size_t iter)
{
	token_valid = false;
	switch (state)
	{
	case START:
		if (input == '=')
		{
			buffer_token.start = iter;
			state = EQ;
		}
		break;
	case EQ:
		if (input == '=')
		{
			state = START;
		}
		else
		{
			CompleteTokenAndSwap("=", line_no, iter - 1);
		}
		break;
	default:
		throw std::logic_error("no such state");
		break;
	}
	return token_valid;
}
