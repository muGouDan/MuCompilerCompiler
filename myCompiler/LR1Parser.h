#pragma once
#include "SetHelper.h"
#include "BaseWord.h"
#include <stack>
#include <vector>
#define SEMANTIC_ERROR (void*)1

//#define LR1_Debug


template<typename T>
class LR1Parser
{
	using Token_Set = std::vector<typename Scanner::Token>;
	using Goto_Table = typename SetHelper<T>::Goto_Table;
	using Action_Table = typename SetHelper<T>::Action_Table;
	using Production_Table = typename SetHelper<T>::Production_Table;
	using Follow_Table = typename SetHelper<T>::Follow_Table;
	using LR1Collection = typename SetHelper<T>::LR1Collection;
	using State = size_t;
	using Transfer_Func = T(*)(const typename Scanner::Token&);

	std::stack<State> state_stack;
	std::stack<void*> semantic_stack;
	std::stack<void*> help_stack;
	std::vector<void*> pass;

	Production_Table production_table;
	Follow_Table follow_table;
	LR1Collection item_collection;
	Goto_Table goto_table;
	Action_Table action_table;
	size_t token_pointer;
public:
	std::string information;

	LR1Parser() :
		token_pointer(0)
	{}

	LR1Parser(
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
		auto back = SetHelper<T>::COLLECTION_LR(
			production_table, first_table, epsilon, end_symbol, first);
		item_collection = std::get<0>(back);
		goto_table = std::get<1>(back);
		action_table = SetHelper<T>::SetActionTable(
			production_table, item_collection, goto_table, follow_table,
			epsilon, end_symbol, first);
		state_stack.push(0);
	}
	bool Parse(const Token_Set& token_set, Transfer_Func trans_func);

	//void(semantic_action)(Parent*, size_t nonterm, size_t pro_index, size_t token_iter)
	template<typename Parent>
	bool Parse(const Token_Set& token_set, Transfer_Func trans_func,
		void(semantic_action)(Parent*, size_t, size_t, size_t), Parent* ptr);

	template<typename Parent>
	bool Parse(const Token_Set& token_set,
		T(trans_func)(Parent*, const Scanner::Token&), Parent* ptr);

	template<typename Parent>
	bool Parse(const Token_Set& token_set,
		T(trans_func)(Parent*, const Scanner::Token&),
		void(semantic_action)(Parent*, size_t, size_t, size_t), Parent* ptr);

	template<typename Parent>
	bool Parse(const Token_Set& token_set,
		T(trans_func)(Parent*, const Scanner::Token&),
		void* (semantic_action)(Parent*, std::vector<void*>, size_t, size_t, size_t), Parent* ptr);

	template<typename Parent>
	bool Parse(const Token_Set& token_set,
		T(trans_func)(Parent*, const Scanner::Token&),
		void* (semantic_action)(Parent*, std::vector<void*>, size_t, size_t, size_t),
		void (error_action)(Parent*, std::vector<T> expects, size_t token_iter), Parent* ptr);
};

template<typename T>
LR1Parser<T>::LR1Parser(
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
	auto back = SetHelper<T>::COLLECTION_LR(
		production_table,first_table, epsilon, end_symbol, first);
	item_collection = std::get<0>(back);
	goto_table = std::get<1>(back);
	action_table = SetHelper<T>::SetActionTable(
		production_table, item_collection, goto_table, follow_table,
		epsilon, end_symbol, first);
	state_stack.push(0);
}

template<typename T>
bool LR1Parser<T>::Parse(const Token_Set& token_set, Transfer_Func trans_func)
{
	auto len = token_set.size();
	size_t iter = 0;
	bool on = true;
	bool acc = false;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input_term = trans_func(token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input_term });
		if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
#ifdef LR1_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef LR1_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // LR1_Debug
				break;
			}
			case ActionType::accept:
				on = false;
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
				std::cout << information << ":LR1Parse Accept" << std::endl;
				SetConsoleColor();
				acc = true;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << information << ":LR1Parse Error" << std::endl;
			std::cout << "Stack Top: " << state_stack.top() << "\n"
				<< token_set[iter] << std::endl;
			break;
		}
	}
	return acc;
}

template<typename T>
template<typename Parent>
bool LR1Parser<T>::Parse(const Token_Set& token_set, Transfer_Func trans_func,
	void(semantic_action)(Parent*, size_t, size_t, size_t), Parent* ptr)
{
	auto len = token_set.size();
	size_t iter = 0;
	bool on = true;
	bool acc = false;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input_term = trans_func(token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input_term });
		if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
#ifdef LR1_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::move_epsilon:
				state_stack.push(action.aim_state);
				semantic_stack.push(nullptr);
#ifdef LR1_Debug
				std::cout << "move_epsilon state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef LR1_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // LR1_Debug
				semantic_action(ptr, (size_t)action.sym, action.production_index, iter - 1);
				break;
			}
			case ActionType::accept:
				on = false;
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
				std::cout << information << ":LR1Parse Accept" << std::endl;
				SetConsoleColor();
				acc = true;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << information << ":LR1Parse Error" << std::endl;
			std::cout << "Stack Top: " << state_stack.top() << "\n"
				<< token_set[iter] << std::endl;
			break;
		}
	}
	return acc;
}

template<typename T>
template<typename Parent>
bool LR1Parser<T>::Parse(const Token_Set& token_set,
	T(trans_func)(Parent*, const Scanner::Token&), Parent* ptr)
{
	auto len = token_set.size();
	size_t iter = 0;
	bool on = true;
	bool acc = false;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input_term = trans_func(ptr, token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input_term });
		if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				++iter;
#ifdef LR1_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef LR1_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // LR1_Debug
				break;
			}
			case ActionType::accept:
				on = false;
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
				std::cout << information << ":LR1Parse Accept" << std::endl;
				SetConsoleColor();
				acc = true;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << information << ":LR1Parse Error" << std::endl;
			std::cout << "Stack Top: " << state_stack.top() << "\n"
				<< token_set[iter] << std::endl;
			break;
		}
	}
	return acc;
}

template<typename T>
template<typename Parent>
bool LR1Parser<T>::Parse(const Token_Set& token_set,
	T(trans_func)(Parent*, const Scanner::Token&),
	void(semantic_action)(Parent*, size_t, size_t, size_t), Parent* ptr)
{
	auto len = token_set.size();
	size_t top_token_iter = 0;
	size_t iter = 0;
	bool on = true;
	bool acc = false;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input_term = trans_func(ptr, token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input_term });
		if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				top_token_iter = iter;
				++iter;
#ifdef LR1_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::reduce:
			{
				for (size_t i = 0; i < action.production_length; i++)
					state_stack.pop();
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef LR1_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // LR1_Debug
				semantic_action(ptr, (size_t)action.sym, action.production_index, top_token_iter);
				break;
			}
			case ActionType::accept:
				on = false;
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
				std::cout << information << ":LR1Parse Accept" << std::endl;
				SetConsoleColor();
				acc = true;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << information << ":LR1Parse Error" << std::endl;
			std::cout << "Stack Top: " << state_stack.top() << "\n"
				<< token_set[iter] << std::endl;
			break;
		}
	}
	return acc;
}

template<typename T>
template<typename Parent>
bool LR1Parser<T>::Parse(
	const Token_Set& token_set,
	T(trans_func)(Parent*, const Scanner::Token&),
	void* (semantic_action)(Parent*, std::vector<void*>, size_t, size_t, size_t),
	Parent* ptr)
{
	auto len = token_set.size();
	size_t top_token_iter = 0;
	size_t iter = 0;
	bool on = true;
	bool acc = false;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input_term = trans_func(ptr, token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input_term });
		if (action_table.find({ state_stack.top(),input_term }) != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				semantic_stack.push(nullptr);
				top_token_iter = iter;
				++iter;
#ifdef LR1_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::move_epsilon:
				state_stack.push(action.aim_state);
				semantic_stack.push(nullptr);
				top_token_iter = iter;
#ifdef LR1_Debug
				std::cout << "move_epsilon state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::reduce:
			{
				pass.clear();
				for (size_t i = 0; i < action.production_length; i++)
				{
					state_stack.pop();
					help_stack.push(semantic_stack.top());
					semantic_stack.pop();
				}
				for (size_t i = 0; i < action.production_length; i++)
				{
					pass.push_back(help_stack.top());
					help_stack.pop();
				}
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef LR1_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // LR1_Debug
				semantic_stack.push(
					semantic_action(ptr, std::move(pass), (size_t)action.sym, action.production_index, top_token_iter)
				);
				break;
			}
			case ActionType::accept:
				pass.clear();
				pass.push_back(semantic_stack.top());
				semantic_stack.pop();
				on = false;
				semantic_stack.push(
					semantic_action(ptr, std::move(pass), (size_t)action.sym, action.production_index, top_token_iter)
				);
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
				std::cout << information << ":LR1Parse Accept" << std::endl;
				SetConsoleColor();
				acc = true;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << information << ":LR1Parse Error" << std::endl;
			std::cout << "Stack Top: " << state_stack.top() << "\n"
				<< token_set[iter] << std::endl;
			break;
		}
	}
	return acc;
}

template<typename T>
template<typename Parent>
bool LR1Parser<T>::Parse(const Token_Set& token_set,
	T(trans_func)(Parent*, const Scanner::Token&),
	void* (semantic_action)(Parent*, std::vector<void*>, size_t, size_t, size_t),
	void (error_action)(Parent*, std::vector<T> expects, size_t token_iter), Parent* ptr)
{
	auto len = token_set.size();
	size_t top_token_iter = 0;
	size_t iter = 0;
	bool on = true;
	bool acc = false;
	while (on)
	{
		if (iter >= token_set.size())
		{
			std::cout << "tokens run out without a End_Separator" << std::endl;
			break;
		}
		T input_term = trans_func(ptr, token_set[iter]);
		auto action_iter = action_table.find({ state_stack.top(),input_term });
		if (action_iter != action_table.end())
		{
			auto& action = std::get<1>(*action_iter);
			switch (action.type)
			{
			case ActionType::move_in:
				state_stack.push(action.aim_state);
				semantic_stack.push(nullptr);
				top_token_iter = iter;
				++iter;
#ifdef LR1_Debug
				std::cout << "move_in state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::move_epsilon:
				state_stack.push(action.aim_state);
				semantic_stack.push(nullptr);
				top_token_iter = iter;
#ifdef LR1_Debug
				std::cout << "move_epsilon state:" << action.aim_state
					<< " term: " << action.sym << std::endl;
#endif // LR1_Debug
				break;
			case ActionType::reduce:
			{
				pass.clear();
				for (size_t i = 0; i < action.production_length; i++)
				{
					state_stack.pop();
					help_stack.push(semantic_stack.top());
					semantic_stack.pop();
				}
				for (size_t i = 0; i < action.production_length; i++)
				{
					pass.push_back(help_stack.top());
					help_stack.pop();
				}
				auto goto_state = goto_table[{state_stack.top(), action.sym}];
				state_stack.push(goto_state);
#ifdef LR1_Debug
				std::cout << "reduce: nonterm" << action.sym << "\n"
					<< "\tpop amount: " << action.production_length
					<< " push state: " << goto_state << std::endl;
#endif // LR1_Debug
				auto back = semantic_action(ptr, std::move(pass), (size_t)action.sym, action.production_index, top_token_iter);
				if (back == SEMANTIC_ERROR)
				{
					on = false;
					std::cout << information << ":LR1Parse STOP for semantic Error" << std::endl;
				}
				else
					semantic_stack.push(back);
				break;
			}
			case ActionType::accept:
				pass.clear();
				pass.push_back(semantic_stack.top());
				semantic_stack.pop();
				on = false;
				semantic_stack.push(
					semantic_action(ptr, std::move(pass), (size_t)action.sym, action.production_index, top_token_iter)
				);
				SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
				std::cout << information << ":LR1Parse Accept" << std::endl;
				SetConsoleColor();
				acc = true;
				break;
			default:
				assert("Fatal Error!");
				break;
			}
		}
		else
		{
			std::cout << information << ":LR1Parse Error" << std::endl;
			std::vector<T> expects;
			for (const auto& item : action_table)
				if (std::get<0>(item.first) == state_stack.top())
					expects.push_back(std::get<1>(item.first));
			error_action(ptr, std::move(expects), iter);
			break;
		}
	}
	return acc;
}
