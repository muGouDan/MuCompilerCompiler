#include "BaseWord.h"
#define Case(element) case element: type_name = #element; break
namespace Scanner
{
	std::ostream& operator << (std::ostream& out, const Token& token)
	{
		std::string type_name;
		switch (token.type)
		{
			Case(TokenType::none);
			Case(TokenType::rel_op);
			Case(TokenType::log_op);
			Case(TokenType::arith_op);
			Case(TokenType::keyword);
			Case(TokenType::identifier);
			Case(TokenType::digit);
			Case(TokenType::assign);
			Case(TokenType::separator);
			Case(TokenType::raw_string);
			Case(TokenType::end_symbol);	
		default:
			break;
		}
		return std::cout
			<< "type: " << type_name << "\n"
			<< "name: " << token.name << "\n"
			//<< "hash: " << token.encode << "\n"
			<< "line: " << token.line << " "<< "start: " << token.start << " "<< "end: " << token.end
			;
	}
}
