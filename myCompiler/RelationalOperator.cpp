#include "RelationalOperator.h"
bool Scanner::RelationalOperator::Scann(char input, const size_t line_no, const size_t iter,char next)
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
				if (last_input != '-')
				{
					state = SINGLE_GREATER;
					buffer_token.start = iter;
				}
				else
				{
					state = START;
				}			
				break;
			case '=':
				state = SINGLE_EQ;
				buffer_token.start = iter;
				break;
			case '!':
				state = SINGLE_NE;
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
			case '=':
				CompleteTokenAndSwap("<=", line_no, iter);
				break;
			default:
				if (input != '<')
				{
					CompleteTokenAndSwap("<", line_no, iter - 1);
					roll_back = true;
				}
				else
				{
					state = START;
				}
				break;
			}
			break;
		case SINGLE_GREATER:
			switch (input)
			{
			case '=':
				CompleteTokenAndSwap(">=", line_no, iter);
				break;
			default:
				if (input != '>')
				{
					CompleteTokenAndSwap(">", line_no, iter - 1);
					roll_back = true;
				}
				else
				{
					state = START;
				}
				break;
			}
			break;
		case SINGLE_EQ:
			switch (input)
			{
			case '=':
				CompleteTokenAndSwap("==", line_no, iter);
				break;
			default:
				state = START;
				roll_back = true;
				break;
			}
			break;
		case SINGLE_NE:
			switch (input)
			{
			case '=':
				CompleteTokenAndSwap("!=", line_no, iter);
				break;
			default:
				state = START;
				roll_back = true;
				break;
			}
			break;
		default:
			throw(std::logic_error("no such state!"));
			break;
		}
	}
	last_input = input;
	return token_valid;
}
//switch (input)
//{
//case '<'://front+'<'
//	if (state == START)//''+'<'
//	{
//		state = SINGLE_LESS;
//		buffer_token.start = iter;
//	}
//	else //other+'<'
//	{
//		state = START;
//		roll_back = true;
//	}
//	break;
//case '>'://front+'>'
//	if (state == START)//''
//	{
//		state = SINGLE_GREATER;//'>'
//		buffer_token.start = iter;
//	}
//	else
//	{
//		state = START;
//		roll_back = true;
//	}
//	break;
//case '='://front+'='
//	switch (state)
//	{
//	case START:
//		state = SINGLE_EQ;
//		buffer_token.start = iter;
//		break;
//	case SINGLE_LESS:
//		CompleteTokenAndSwap("<=", line_no, iter);
//		break;
//	case SINGLE_GREATER:
//		CompleteTokenAndSwap(">=", line_no, iter);
//		break;
//	case SINGLE_EQ:
//		CompleteTokenAndSwap("==", line_no, iter);
//		break;
//	case SINGLE_NE:
//		CompleteTokenAndSwap("!=", line_no, iter);
//		break;
//	default:
//		state = START;
//		roll_back = true;
//		break;
//	}
//	break;
//case '!'://front+'!'
//	if (state == START)
//	{
//		state = SINGLE_NE;
//		buffer_token.start = iter;
//	}
//	else
//	{
//		state = START;
//		roll_back = true;
//	}
//	break;
//default://front+other
//	switch (state)
//	{
//	case SINGLE_LESS://'<'+other
//		CompleteTokenAndSwap("<", line_no, iter - 1);
//		roll_back = true;
//		break;
//	case SINGLE_GREATER://'>'+other
//		CompleteTokenAndSwap(">", line_no, iter - 1);
//		roll_back = true;
//		break;
//	default://''+other
//		state = START;
//		//kick out
//		break;
//	}
//	break;
//}