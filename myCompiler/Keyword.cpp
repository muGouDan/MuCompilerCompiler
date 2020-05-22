#include "Keyword.h"


void Scanner::Keyword::LoadConfig()
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
			pairs.push_back(std::make_tuple(str_helper, 0, 0));
			str_helper.clear();
		}		
	}
	fs.close();
}

bool Scanner::Keyword::Scann(char input, const size_t line_no, const size_t iter)
{
	if (input == '\"') str_ = !str_;
	else if (input == '\'') char_ = !char_;
	if (char_ || str_) return false;
	for (auto& pair : pairs)
	{
		if (std::get<1>(pair) == OFF)
			continue;
		if (std::get<0>(pair).size() == std::get<2>(pair))
		{
			if (!std::isalnum(input) && input != '_')
			{
				SetBufferToken(std::get<0>(pair).c_str(), line_no, iter - 1);
				++finished_amount;
				raw_valid = true;
			}
			else
			{
				std::get<1>(pair) = OFF;
				++finished_amount;
				continue;
			}
		}
		else if(std::get<0>(pair)[std::get<2>(pair)] == input)
		{
			if (std::get<1>(pair) == START)
			{
				++std::get<2>(pair);
				std::get<1>(pair) = HEAD;
				buffer_token.start = iter;
			}			
			else
			{
				std::get<1>(pair) = HEAD;
				++std::get<2>(pair);
			}
		}
		else
		{
			std::get<1>(pair) = OFF;
			++finished_amount;
		}
	}
	if (finished_amount == pairs.size())
	{
		if (raw_valid)
		{
			auto temp = std::move(buffer_token);
			buffer_token = std::move(current_token);
			current_token = std::move(temp);
			state = START;
			token_valid = true;		
		}
		else
		{
			token_valid = false;
		}		
		RefleshPairs();
	}
	else
	{
		token_valid = false;
	}
	return token_valid;
}
