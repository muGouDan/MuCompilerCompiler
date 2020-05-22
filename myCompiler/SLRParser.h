#pragma once
#include "SetHelper.h"
#include "BaseWord.h"
#include <stack>
#include <vector>
#define SLR_Debug
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
	SLRParser() :
		token_pointer(0)
	{}

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

	void SetUp(const Production_Table& production_table,
		const T last_term, const T end_symbol,
		const T epsilon, const T first)
	{
		this->production_table = production_table;
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
	void Parse(const Token_Set& token_set_for_production, Transfer_Func trans_func);
	
	//void(syntax_action)(Parent*, size_t nonterm, size_t pro_index, size_t token_iter)
	template<typename Parent>
	void Parse(const Token_Set& token_set_for_production, Transfer_Func trans_func,
		void(syntax_action)(Parent*, size_t, size_t, size_t), Parent* ptr);

	template<typename Parent>
	void Parse(const Token_Set& token_set_for_production,
		T(trans_func)(Parent*, const Scanner::Token&), Parent* ptr);
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
void SLRParser<T>::Parse(const Token_Set& token_set_for_production, Transfer_Func trans_func)
{
	auto len = token_set_for_production.size();
	size_t iter = 0;
	bool on = true;
	while (on)
	{
		if (iter >= token_set_for_production.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input = trans_func(token_set_for_production[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input });
		if (action_table.find({ state_stack.top(),input }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
#ifdef SLR_Debug
				std::cout << "move_in state:" << action.aim_state 
					<< " term: "<< action.sym << std::endl;
#endif // SLR_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef SLR_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // SLR_Debug
				break;
			}
			case ActionType::accept:
				on = false;
#ifdef SLR_Debug
				std::cout << "Accept" << std::endl;
#endif // SLR_Debug
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << "Error Parse:\n"
				<< "Stack Top: " << state_stack.top() << "\n" 
				<< token_set_for_production[iter] << std::endl;
			break;
		}
	}
}



template<typename T>
template<typename Parent>
void SLRParser<T>::Parse(const Token_Set& token_set_for_production, Transfer_Func trans_func, 
	void(syntax_action)(Parent*,size_t,size_t,size_t),Parent* ptr)
{
	auto len = token_set_for_production.size();
	size_t iter = 0;
	bool on = true;
	while (on)
	{
		if (iter >= token_set_for_production.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input = trans_func(token_set_for_production[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input });
		if (action_table.find({ state_stack.top(),input }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
#ifdef SLR_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // SLR_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef SLR_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // SLR_Debug
				syntax_action(ptr, (size_t)action.sym, action.production_index, iter - 1);
				break;
			}
			case ActionType::accept:
				on = false;
#ifdef SLR_Debug
				std::cout << "Accept" << std::endl;
#endif // SLR_Debug
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << "Error Parse:\n"
				<< "Stack Top: " << state_stack.top() << "\n"
				<< token_set_for_production[iter] << std::endl;
			break;
		}
	}
}

template<typename T>
template<typename Parent>
void SLRParser<T>::Parse(const Token_Set& token_set_for_production,
	T(trans_func)(Parent*, const Scanner::Token&), Parent* ptr)
{
	auto len = token_set_for_production.size();
	size_t iter = 0;
	bool on = true;
	while (on)
	{
		if (iter >= token_set_for_production.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input = trans_func(ptr,token_set_for_production[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input });
		if (action_table.find({ state_stack.top(),input }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
#ifdef SLR_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // SLR_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef SLR_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // SLR_Debug
				break;
			}
			case ActionType::accept:
				on = false;
#ifdef SLR_Debug
				std::cout << "Accept" << std::endl;
#endif // SLR_Debug
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << "Error Parse:\n"
				<< "Stack Top: " << state_stack.top() << "\n"
				<< token_set_for_production[iter] << std::endl;
			break;
		}
	}
}
