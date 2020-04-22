#include "RelationalOperator.h"

//Parser::ParseResult Parser::RelativeOperator::Parse(const char* line_src, Token& token, const size_t& line_no, const size_t& start)
//{
//	size_t iter = start;
//	char cur_char = '\0';
//	size_t state = START;
//	ParseResult ret;
//	bool on = true;
//	//for token
//	size_t token_start = -1;
//
//	while (on)
//	{
//		cur_char = line_src[iter];
//		// get the char which isn't space or tab
//		while (cur_char == ' ' || cur_char == '\t') cur_char = line_src[++iter];
//		switch (cur_char)
//		{
//		case '<':
//			if (state == START)//''
//			{
//				state = LESS;//'<'
//				token_start = iter;
//			}
//			else
//			{
//				on = false;
//				ret = ParseResult::error;
//			}
//			break;
//		case '>':
//			if (state == START)//''
//			{
//				state = GREATER;//'>'
//				token_start = iter;
//			}
//			else
//			{
//				on = false;
//				ret = ParseResult::error;
//			}
//			break;
//		case '=':
//			switch (state)
//			{
//			case START:
//				state = EQ;
//				token_start = iter;
//				break;
//			case LESS:
//				on = false;
//				ret = ParseResult::success;
//				token.end = iter;
//				token.name = "<=";
//				break;
//			case GREATER:
//				on = false;
//				ret = ParseResult::success;
//				token.end = iter;
//				token.name = ">=";
//				break;
//			case EQ:
//				on = false;
//				ret = ParseResult::success;
//				token.end = iter;
//				token.name = "==";
//				break;
//			case NE:
//				on = false;
//				ret = ParseResult::success;
//				token.end = iter;
//				token.name = "!=";
//				break;
//			default:
//				on = false;
//				ret = ParseResult::error;
//				break;
//			}
//			break;
//		case '!':
//			if (state == START)
//			{
//				state = NE;
//				token_start = iter;
//			}
//			else
//			{
//				on = false;
//				ret = ParseResult::error;
//			}
//			break;
//		case '\0':
//			if (state == START)
//			{
//				on = false;
//				ret = ParseResult::end_of_file;
//			}
//			else
//			{
//				on = false;
//				ret = (ParseResult)(success | ParseResult::end_of_file);
//				token.name = "<";
//				token.end = --iter;
//			}
//			break;
//		case '\n':
//			if (state == START)
//			{
//				on = false;
//				ret = ParseResult::end_of_line;
//			}
//			else
//			{
//				on = false;
//				ret = (ParseResult)(success | ParseResult::end_of_file);
//				token.name = ">";
//				token.end = --iter;
//			}
//			break;
//		default:
//			switch (state)
//			{
//			case LESS:
//				on = false;
//				ret = ParseResult::success;
//				token.name = "<";
//				token.end = --iter;
//				break;
//			case GREATER:
//				on = false;
//				ret = ParseResult::success;
//				token.name = ">";
//				token.end = --iter;
//			default:
//				break;
//			}
//			break;
//		}
//		++iter;
//	}
//	if (static_cast<int>(ret) & static_cast<int>(ParseResult::success))
//	{
//		token.start = token_start;
//		token.type = TokenType::rel_op;
//		token.line = line_no;
//		token.encode = GetHash(token.name.c_str());
//	}
//	return ret;
//}

bool Scanner::RelationalOperator::Scann(char input, const size_t line_no, const size_t iter)
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
				CompleteTokenAndSwap("<", line_no, iter - 1);
				roll_back = true;
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
				CompleteTokenAndSwap(">", line_no, iter - 1);
				roll_back = true;
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