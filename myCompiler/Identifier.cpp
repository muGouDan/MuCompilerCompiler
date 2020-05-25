#include "Identifier.h"

void Scanner::Identifier::LoadConfig()
{
	std::ifstream fs(KEYWORD_CFG);
	char container[1024];
	while (fs.getline(container, 1024))
	{
		if (container[0] != '#')
		{
			str_helper.append(container);
			if (str_helper == "")
				continue;
			keywords.push_back(str_helper);
			str_helper.clear();
		}
	}
	fs.close();
}

bool Scanner::Identifier::Scann(char input, const size_t line_no, const size_t iter,char next)
{
	token_valid = false;
	bool roll_back = true;
	while (roll_back)
	{
		roll_back = false;
		switch (state)
		{
		case START:
			char_iter = 0;
			char_buffer[0] = '\0';
			if (isalpha(input) || input == '_')
			{
				buffer_token.start = iter;			
				char_buffer[char_iter++] = input;
				state = HEAD;
			}
			break;
		case HEAD:
			if (isalnum(input) || input == '_')
			{
				if (char_iter >= CHAR_BUFFER_MAX)
					throw std::out_of_range("the length identifier out of range!");
				char_buffer[char_iter++] = input;
			}
			else
			{
				if (char_iter >= CHAR_BUFFER_MAX)
					throw std::out_of_range("the length identifier out of range!");
				char_buffer[char_iter++] = '\0';
				bool match = false;
				for (const auto keyword:keywords)
				{
					str_helper = char_buffer;
					if (str_helper == keyword)
					{
						match = true;
						break;
					}
				}
				if (match)
					state = START;	
				else
					CompleteTokenAndSwap(char_buffer, line_no, iter - 1);
			}
			break;
		default:
			throw std::logic_error("no such state");
			break;
		}
	}
	return token_valid;
}
