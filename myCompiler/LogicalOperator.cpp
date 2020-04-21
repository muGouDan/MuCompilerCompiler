#include "LogicalOperator.h"

// << >> ! && || & |
bool Scanner::LogicalOperator::Scann(char input, const size_t line_no, const size_t iter)
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
			case '<':
				state = SINGLE_LESS;
				buffer_token.start = iter;
				break;
			case '>':
				state = SINGLE_GREATER;
				buffer_token.start = iter;
				break;
			case '!':
				buffer_token.start = iter;
				CompleteTokenAndSwap("!", line_no, iter - 1);
			case '&':
				state = SINGLE_AND;
				buffer_token.start = iter;
				break;
			case '|':
				state = SINGLE_OR;
				buffer_token.start = iter;
				break;
			default:
				state = START;
				break;
			}
			break;
		case SINGLE_LESS:
			switch (input)
			{
			case '<':
				CompleteTokenAndSwap("<<", line_no, iter);
				break;
			default:
				state = START;
				break;
			}
			break;
		case SINGLE_GREATER:
			switch (input)
			{
			case '>':
				CompleteTokenAndSwap(">>", line_no, iter);
				break;
			default:
				state = START;
				break;
			}
			break;
		case SINGLE_AND:
			switch (input)
			{
			case '&':
				CompleteTokenAndSwap("&&", line_no, iter);
				break;
			default:
				CompleteTokenAndSwap("&", line_no, iter - 1);
				roll_back = true;
				break;
			}
			break;
		case SINGLE_OR:
			switch (input)
			{
			case '|':
				CompleteTokenAndSwap("||", line_no, iter - 1);
				break;
			default:
				CompleteTokenAndSwap("|", line_no, iter - 1);
				roll_back = true;
				break;
			}
			break;
		default:
			throw(std::logic_error("no such state!"));
			break;
		}
	}
	return token_valid;
}