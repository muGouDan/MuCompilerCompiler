#include "ArithmeticExpression.h"

Parser::Production Parser::ArithmeticExpression::table[ROW][COLUMN];

bool Parser::ArithmeticExpression::ready = false;

void Parser::ArithmeticExpression::Parse(std::vector<Scanner::Token>& token_set, size_t start)
{
	std::cout << "ArithmeticExpression Parser: Start" << std::endl;
	//current terminator
	symbol current = symbol::none;
	parser_stack.push(symbol::end);
	parser_stack.push(symbol::E);
	int i = start;
	while (true)
	{
		if (parser_stack.top() == symbol::end)
		{
			std::cout << "ArithmeticExpression Parser: Success";
			break;
		}
		current = GetTerm(token_set[i]);
		if (current != symbol::none)
		{
			symbol var_top = parser_stack.top();
			if (var_top == current)//
			{
				parser_stack.pop();
				++i;
			}
			else if (var_top >= symbol::elem && var_top <= symbol::right_p)
				//top is terminator but doesn't equal to current
			{
				std::cout << "terminator dismatch: \n" << token_set[i] << std::endl;
				break;
			}
			else if (!table[GetIndex(var_top)][GetIndex(current)].valid)
			{
				std::cout << table[GetIndex(var_top)][GetIndex(current)].message << " "
					<< token_set[i] << std::endl;
				break;
			}
			else if (table[GetIndex(var_top)][GetIndex(current)].valid)
			{
				parser_stack.pop();
				for (int i = table[GetIndex(var_top)][GetIndex(current)].production.size() - 1; i >=0; --i)
				{
					auto& Sym = table[GetIndex(var_top)][GetIndex(current)].production[i];
					if (Sym != nil)
					{
						parser_stack.push((symbol)Sym);
					}				
				}				
			}
		}
		else
		{
			//TODO: can't receive such token!
			std::cout << "error" << std::endl;
			break;
		}
	}
}

void Parser::ArithmeticExpression::SetUpTable()
{
	table[GetIndex(E)][GetIndex(elem)] = Production({ T, E_ });
	table[GetIndex(E)][GetIndex(plus_and_minus)] = Production("ERROR");
	table[GetIndex(E)][GetIndex(mul_and_div)] = Production("ERROR");
	table[GetIndex(E)][GetIndex(left_p)] = Production({ T, E_ });
	table[GetIndex(E)][GetIndex(right_p)] = Production({ T, E_ });
	table[GetIndex(E)][GetIndex(end)] = Production("ERROR");

	table[GetIndex(E_)][GetIndex(elem)] = Production("ERROR");
	table[GetIndex(E_)][GetIndex(plus_and_minus)] = Production({ plus_and_minus, T, E_ });
	table[GetIndex(E_)][GetIndex(mul_and_div)] = Production("ERROR");
	table[GetIndex(E_)][GetIndex(left_p)] = Production("ERROR");
	table[GetIndex(E_)][GetIndex(right_p)] = Production({ nil });
	table[GetIndex(E_)][GetIndex(end)] = Production({ nil });

	table[GetIndex(T)][GetIndex(elem)] = Production({ F, T_ });
	table[GetIndex(T)][GetIndex(plus_and_minus)] = Production("ERROR");
	table[GetIndex(T)][GetIndex(mul_and_div)] = Production("ERROR");
	table[GetIndex(T)][GetIndex(left_p)] = Production({ F, T_ });
	table[GetIndex(T)][GetIndex(right_p)] = Production("ERROR");
	table[GetIndex(T)][GetIndex(end)] = Production("ERROR");

	table[GetIndex(T_)][GetIndex(elem)] = Production("ERROR");
	table[GetIndex(T_)][GetIndex(plus_and_minus)] = Production({ nil });
	table[GetIndex(T_)][GetIndex(mul_and_div)] = Production({ mul_and_div, F, T_ });
	table[GetIndex(T_)][GetIndex(left_p)] = Production("ERROR");
	table[GetIndex(T_)][GetIndex(right_p)] = Production({ nil });
	table[GetIndex(T_)][GetIndex(end)] = Production({ nil });

	table[GetIndex(F)][GetIndex(elem)] = Production({ elem });					//elem
	table[GetIndex(F)][GetIndex(plus_and_minus)] = Production("ERROR");
	table[GetIndex(F)][GetIndex(mul_and_div)] = Production("ERROR");
	table[GetIndex(F)][GetIndex(left_p)] = Production({ left_p, E, right_p });	//(E)
	table[GetIndex(F)][GetIndex(right_p)] = Production("ERROR");
	table[GetIndex(F)][GetIndex(end)] = Production("ERROR");

	ready = true;
}
