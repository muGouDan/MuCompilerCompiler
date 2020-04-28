#include "BaseWord.h"
#define Case(element) case TokenType::##element: type_name = (#element); break
namespace Scanner
{
	std::ostream& operator << (std::ostream& out, const Token& token)
	{
		std::string type_name;
		switch (token.type)
		{
			Case(none);
			Case(rel_op);
			Case(log_op);
			Case(arith_op);
			Case(keyword);
			Case(identifier);
			Case(digit);
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
