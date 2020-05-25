#include "RawString.h"

bool Scanner::RawString::Scann(char input, const size_t line_no, const size_t iter,char next)
{
	token_valid = false;
	switch (state)
	{
	case START:
		if (input == '\'')
		{
			buffer_token.start = iter;
			state = CHAR_ON;
		}
		else if (input == '\"')
		{
			buffer_token.start = iter;
			state = STR_ON;
		}
		break;
	case CHAR_ON:
		if (input == '\"')
		{
			state = START;
			break;
		}
		if (input != '\'')
		{
			buffer[buffer_iter] = input;
			buffer[buffer_iter + 1] = '\0';
			state = CHAR_BODY;
		}
		else
		{
			CompleteTokenAndSwap(buffer, line_no, iter);
			buffer[0] = '\0';
			buffer_iter = 0;
		}
		break;
	case CHAR_BODY:
		if (input == '\'')
		{
			CompleteTokenAndSwap(buffer, line_no, iter);
			buffer[0] = '\0';
			buffer_iter = 0;
		}			
		state = START;
		break;
	case STR_ON:
		if (input == '\"')
		{
			buffer[buffer_iter] = '\0';
			CompleteTokenAndSwap(buffer, line_no, iter);
			buffer[0] = '\0';
			buffer_iter = 0;
		}
		else
		{
			buffer[buffer_iter++] = input;
		}
		break;
	default:
		break;
	}
	return token_valid;
}
