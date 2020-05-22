#include "Separator.h"

bool Scanner::Separator::Scann(char input, const size_t line_no, const size_t iter)
{
	token_valid = false;
	switch (state)
	{
	case START:
		switch (input)
		{
		case ';':
			buffer_token.start = iter;
			CompleteTokenAndSwap(";", line_no, iter);
			break;
		case '{':
			buffer_token.start = iter;
			CompleteTokenAndSwap("{", line_no, iter);
			break;
		case '}':
			buffer_token.start = iter;
			CompleteTokenAndSwap("}", line_no, iter);
			break;
		case '[':
			buffer_token.start = iter;
			CompleteTokenAndSwap("[", line_no, iter);
			break;
		case ']':
			buffer_token.start = iter;
			CompleteTokenAndSwap("]", line_no, iter);
			break;
		case ',':
			buffer_token.start = iter;
			CompleteTokenAndSwap(",", line_no, iter);
			break;
		case '.':
			buffer_token.start = iter;
			state = SINGLE_POINT;
			break;
		case ':':
			buffer_token.start = iter;
			state = SINGLE_COLON;
			break;
		case '-':
			buffer_token.start = iter;
			state = SINGLE_MINUS;
			break;
		default:
			break;
		}
		break;
	case SINGLE_COLON:
		if (input == ':')
			CompleteTokenAndSwap("::", line_no, iter);
		else
			state = START;
		break;
	case SINGLE_MINUS:
		if (input == '>')
			CompleteTokenAndSwap("->", line_no, iter);
		else
			state = START;
		break;
	case SINGLE_POINT:
		if (!isdigit(input))
			CompleteTokenAndSwap(".", line_no, iter - 1);
		else
			state = START;
		break;
	default:
		throw std::logic_error("no such state");
		break;
	}
	return token_valid;
}
