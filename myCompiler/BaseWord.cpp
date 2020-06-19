#include "BaseWord.h"
#define SELECT_TOKENTYPE(token_type) if(#token_type==str) ret = TokenType::token_type; else
#define END_SELECT ;
#define Case(element) case element: type_name = #element; break
namespace Scanner
{
	std::string TokenTypeToString(TokenType type)
	{
		std::string type_name;
		switch (type)
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
			Case(TokenType::custom_type);
			Case(TokenType::end_symbol);
		default:
			break;
		}
		return type_name;
	}
	TokenType StringToTokenType(std::string str)
	{
		TokenType ret = TokenType::none;
		SELECT_TOKENTYPE(rel_op)
		SELECT_TOKENTYPE(log_op)
		SELECT_TOKENTYPE(arith_op)
		SELECT_TOKENTYPE(keyword)
		SELECT_TOKENTYPE(identifier)
		SELECT_TOKENTYPE(digit)
		SELECT_TOKENTYPE(assign)
		SELECT_TOKENTYPE(separator)
		SELECT_TOKENTYPE(raw_string)
		SELECT_TOKENTYPE(custom_type)
		SELECT_TOKENTYPE(end_symbol)
		END_SELECT
		return ret;
	}
	std::ostream& operator << (std::ostream& out, const Token& token)
	{
		std::string type_name = TokenTypeToString(token.type);
		return out
			<< "type: " << type_name << "\n"
			<< "name: " << token.name << "\n"
			//<< "hash: " << token.encode << "\n"
			<< "line: " << token.line << " "<< "start: " << token.start << " "<< "end: " << token.end;
	}
}
