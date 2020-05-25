#include "Digit.h"

bool Scanner::Digit::Scann(char input, const size_t line_no, const size_t iter,char next)
{
	token_valid = false;
	if (last_char == '_' || isalpha(last_char))
	{
		last_char = input;
		return false;
	}	
	switch (state)
	{
	case START:
		char_iter = 0;
		char_buffer[0] = '\0';
		if (input == '0')
		{
			buffer_token.start = iter;
			state = BEGIN_WITH_0;
			char_buffer[char_iter++] = input;
		}
		else if ('1' <= input && input <= '9')
		{
			if (char_iter >= CHAR_BUFFER_MAX)
				throw std::out_of_range("the length identifier out of range!");
			buffer_token.start = iter;
			state = NOT_BEGIN_WITH_0;
			char_buffer[char_iter++] = input;
		}
		break;
	case BEGIN_WITH_0:
		if (input == '.')
		{
			if (char_iter >= CHAR_BUFFER_MAX)
				throw std::out_of_range("the length identifier out of range!");
			state = AFTER_ONE_POINT;
			char_buffer[char_iter++] = input;
		}
		else if(input < '0' || input > '9')
		{
			char_buffer[char_iter] = '\0';
			CompleteTokenAndSwap(char_buffer, line_no, iter - 1);
		}
		break;
	case NOT_BEGIN_WITH_0:
		if ('0' <= input && input <= '9')
		{
			if (char_iter >= CHAR_BUFFER_MAX)
				throw std::out_of_range("the length identifier out of range!");
			char_buffer[char_iter++] = input;
		}
		else if (input == '.')
		{
			if (char_iter >= CHAR_BUFFER_MAX)
				throw std::out_of_range("the length identifier out of range!");
			state = AFTER_ONE_POINT;
			char_buffer[char_iter++] = input;
		}
		else
		{
			char_buffer[char_iter] = '\0';
			CompleteTokenAndSwap(char_buffer, line_no, iter - 1);
		}
		break;
	case AFTER_ONE_POINT:
		if ('0' <= input && input <= '9')
		{
			if (char_iter >= CHAR_BUFFER_MAX)
				throw std::out_of_range("the length identifier out of range!");
			char_buffer[char_iter++] = input;
			state = AFTER_ONE_POINT_HAS_NUM;
		}
		else
		{
			throw std::logic_error("Error Digit");
		}
		break;
	case AFTER_ONE_POINT_HAS_NUM:
		if ('0' <= input && input <= '9')
		{
			if (char_iter >= CHAR_BUFFER_MAX)
				throw std::out_of_range("the length identifier out of range!");
			char_buffer[char_iter++] = input;
		}
		else
		{
			char_buffer[char_iter] = '\0';
			CompleteTokenAndSwap(char_buffer, line_no, iter - 1);
		}
		break;
	default:
		throw std::logic_error("no such state");
		break;
	}
	last_char = input;
	return token_valid;
}
