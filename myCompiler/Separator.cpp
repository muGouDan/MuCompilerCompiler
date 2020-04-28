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
		case ':':
			buffer_token.start = iter;
			state = SINGLE_COLON;
			break;
		default:
			break;
		}
		break;
	case SINGLE_COLON:
		CompleteTokenAndSwap("::", line_no, iter);
		break;
	default:
		throw std::logic_error("no such state");
		break;
	}
	return token_valid;
}
