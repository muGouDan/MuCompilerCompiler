#pragma once
#include "SetHelper.h"
#include "BaseWord.h"
#include <stack>
#include <vector>

template<typename T>
class SLRParser
{
	using Token_Set = std::vector<typename Scanner::Token>;
	using Goto_Table = typename SetHelper<T>::Goto_Table;
	using Action_Table = typename SetHelper<T>::Action_Table;
	using Production_Table = typename SetHelper<T>::Production_Table;
	using Follow_Table = typename SetHelper<T>::Follow_Table;
	using CollectionOfItemSets = typename SetHelper<T>::CollectionOfItemSets;
	using State = size_t;
	using Transfer_Func = T(*)(const typename Scanner::Token&);
	
	std::stack<State> state_stack;
	Production_Table production_table;
	Follow_Table follow_table;
	CollectionOfItemSets item_collection;
	Goto_Table goto_table;
	Action_Table action_table;
	size_t token_pointer;

public:
	SLRParser(
		const Production_Table& production_table,
		const T last_term, const T end_symbol,
		const T epsilon, const T first = (T)0);

	void Reset()
	{
		while (!state_stack.empty())
			state_stack.pop();
		state_stack.push(0);
	}

	void Parse(const Token_Set& token_set, Transfer_Func trans_func);
};

template<typename T>
SLRParser<T>::SLRParser(
	const Production_Table& production_table,
	const T last_term, const T end_symbol, 
	const T epsilon, const T first) :
	token_pointer(0),
	production_table(production_table)
{
	auto first_table = SetHelper<T>::FIRST(
		production_table, epsilon, last_term, first);
	follow_table = SetHelper<T>::FOLLOW(
		first_table, production_table, epsilon, end_symbol, first);
	auto back = SetHelper<T>::COLLECTION(
		production_table, epsilon, end_symbol, first);
	item_collection = std::get<0>(back);
	goto_table = std::get<1>(back);
	action_table = SetHelper<T>::SetActionTable(
		production_table, item_collection, goto_table, follow_table,
		epsilon, end_symbol, first);
	state_stack.push(0);
}

template<typename T>
void SLRParser<T>::Parse(const Token_Set& token_set, Transfer_Func trans_func)
{
	auto len = token_set.size();
	size_t iter = 0;
	bool on = true;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a separator" << std::endl;
			break;
		}
		T input = trans_func(token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input });
		if (action_table.find({ state_stack.top(),input }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
				std::cout << "move_in state:" << action.aim_state << std::endl;
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.nonterm}];
				state_stack.push(goto_state);
				std::cout << "reduce: nonterm" << action.nonterm << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
				break;
			}
			case ActionType::accept:
				on = false;
				std::cout << "Accept" << std::endl;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << "Error Parse:\n"
				<< token_set[iter] << std::endl;
			break;
		}
	}
}
