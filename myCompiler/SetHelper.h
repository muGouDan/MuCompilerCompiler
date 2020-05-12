#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <cassert>
enum class ActionType
{
	error = -1,
	accept,
	move_in,
	reduce
};

template <typename T>
class SetHelper
{
public:
	// A -> ¦Á
	using Production = std::vector<T>;

	// A -> ¦Á0|¦Á1|...
	using Productions = std::vector<Production>;

	// A -> ¦Á0|¦Á1|...
	// B -> ¦Â0|¦Â2|...
	// ...
	using Production_Table = std::vector<Productions>;

	// A: a0,a1,...
	// B: b0,b1,...
	// ...
	using First_Table = std::vector<std::set<T>>;
	using Follow_Table = std::vector<std::set<T>>;

	// SELECT(A -> ¦Á) = {a1,a2,...}
	// Select_Table[nonterm][production] => set of term;
	using Select_Table = std::vector<std::map<Production&, std::set<T>>>;

	//	|nt/t	|a		|b		|c		|
	//	|A		|A->¦Á	|none	|none	|
	//	|B		|none	|none	|B->¦Â	|
	//  |...	|		|		|		|
	using LL1_Table = std::vector<std::vector<Production>>;

	// e.g. [S' -> .S]
	struct Item
	{
		// to get Productions in Production_Table
		T nonterm;
		// to get certain production
		int production_index;
		// A->¦Á.B¦Â
		int point_pos;

		bool my_equals(const Item& obj) const
		{
			return (int)nonterm == (int)obj.nonterm
				&& production_index == obj.production_index
				&& point_pos == obj.point_pos;
		}

		bool operator < (const Item& rhs) const
		{
			if (nonterm < rhs.nonterm) return true;
			if (nonterm == rhs.nonterm
				&& production_index < rhs.production_index)
				return true;
			if (nonterm == rhs.nonterm
				&& production_index == rhs.production_index
				&& point_pos < rhs.point_pos)
				return true;
			return false;
		}
	};

	struct SimplifiedSetOfItems
	{
		SimplifiedSetOfItems(const std::set<Item>& cores, size_t non_core_size) :
			cores(cores), non_cores(non_core_size, false)
		{}

		// e.g. {[E'->E.],[E->E. + T]} 
		std::set<Item> cores;

		// e.g.	include		false	true	false	...
		//		nonterm		A		B		C		...
		std::vector<bool> non_cores;

		bool my_equals(const SimplifiedSetOfItems& set) const
		{
			if (set.cores.size() != cores.size()) return false;
			bool eq = false;
			auto it1 = set.cores.begin();
			auto it2 = cores.begin();
			for (; it1 != set.cores.end();)
			{
				eq = it2->my_equals(*it1);
				if (!eq)
					break;
				++it1;
				++it2;
			}
			return eq;
		}
	};

	// for LR
	using CollectionOfItemSets = std::vector<SimplifiedSetOfItems>;

	// GOTO[state,symbol] -> next state
	using Goto_Table = std::map<std::tuple<size_t, T>, size_t>;

	// type == ActionType::move_in		aim_state available
	// type == ActionType::reduce		nonterm,production_length available 
	// type == ActionType::accept		--
	// type == ActionType::error		--
	struct Action
	{
		ActionType type = ActionType::error;
		union 
		{
			size_t aim_state = 0;
			T nonterm;
		};
		size_t production_length = 0;
	};

	// Action_Table for SLR
	using Action_Table = std::map<std::tuple<size_t, T>, Action>;


	static void Show(const std::vector<std::set<T>>& table)
	{
		for (size_t i = 0; i < table.size(); i++)
		{
			std::cout << "nt[" << i << "]\t:";
			for (auto item : table[i])
			{
				std::cout << "t[" << item << "]\t";
			}
			std::cout << std::endl;
		}
	}
	static void Show(const LL1_Table& table)
	{
		std::cout << "nt/t\t\t";
		for (size_t j = 0; j < table[0].size(); ++j)
		{
			std::cout << "t[" << j + table[0].size() << "]\t\t";
		}
		std::cout << std::endl;
		for (size_t i = 0; i < table.size(); ++i)
		{
			std::cout << "nt[" << i << "]\t\t";
			size_t flag = 0;
			for (size_t j = 0; j < table[i].size(); ++j)
			{
				for (size_t num = flag; num < j; ++num)
				{
					std::cout << "\t\t";
				}
				flag = j;
				for (auto item : table[i][j])
				{
					std::cout << item << " ";
				}
			}
			std::cout << std::endl;
		}
	}

	static First_Table FIRST(
		const Production_Table& production_table,
		const T epsilon, const T last_term, const T first = (T)0)
	{
		auto int_last = (int)last_term;
		auto int_first = (int)first;
		First_Table ret(int_last + 1);
		bool changed = true;
		while (changed)
		{
			changed = false;
			//do until First_Table no longer changes;
			for (int i = int_last; i >= int_first; --i)
				setup_first_table(changed, ret, production_table, epsilon, (T)i);
		}
		return ret;
	}

	static Follow_Table FOLLOW(
		const First_Table& first_table,
		const Production_Table& production_table,
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0);

	static Select_Table SELECT(
		const First_Table& first_table,
		const Follow_Table& follow_table,
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0)
	{
		//TODO:
		//auto int_first_of_nonterm = (int)first_nonterm;
		//auto int_end_of_nonterm = (int)epsilon - 1;
		//Select_Table ret(int_end_of_nonterm + 1);
	}

	static LL1_Table Preanalysis(
		const First_Table& first_table,
		const Follow_Table& follow_table,
		const Production_Table& production_table,
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0);

	static SimplifiedSetOfItems CLOSURE(
		const Production_Table& production_table,
		const std::set<Item> cores,
		const T& epsilon, const T& start = (T)0);

	static SimplifiedSetOfItems CLOSURE(
		const Production_Table& production_table,
		const SimplifiedSetOfItems& I,
		const T& epsilon, const T& start = (T)0)
	{
		return CLOSURE(production_table, I.cores, epsilon, start);
	}

	static SimplifiedSetOfItems GOTO(
		const Production_Table& production_table,
		const SimplifiedSetOfItems& I,
		const T& symbol,
		const T& epsilon, const T& start = (T)0);

	static std::tuple<CollectionOfItemSets, Goto_Table> COLLECTION(
		const Production_Table& production_table,
		const T& epsilon, const T& end, const T& start = (T)0);

	static Action_Table SetActionTable(
		const Production_Table& production_table,
		const CollectionOfItemSets& collection,
		const Goto_Table& goto_table,
		const Follow_Table& follow_table,
		const T& epsilon, const T& end, const T& start = (T)0);
private:
	// term > epsilon > nonterm
	static void setup_first_table(
		bool& changed,
		First_Table& first_table,
		const Production_Table& production_table,
		const T epsilon, const T sym);

	// for certain A -> ¦ÁB¦Â
	// beta_start_index>=length of production X->¦ÁB¦Â,means X -> ¦ÁB
	static void set_follow_of_X_with_certain_beta(
		bool& changed,
		const First_Table& first_table,
		Follow_Table& follow_table,
		const Production_Table& production_table,
		const T epsilon, const T sym_A,
		const int production_index, const int pos_B);
};

template<typename T>
typename SetHelper<T>::Follow_Table SetHelper<T>::FOLLOW(
	const First_Table& first_table,
	const Production_Table& production_table,
	const T epsilon, const T end_symbol, const T first_nonterm)
{
	auto int_first_of_nonterm = (int)first_nonterm;
	auto int_end_of_nonterm = (int)epsilon - 1;
	Follow_Table ret(int_end_of_nonterm + 1);
	ret[int_first_of_nonterm].insert(end_symbol);
	bool changed = true;
	while (changed)
	{
		changed = false;
		//do until Follow_Table no longer changes;
		for (int non_term = int_first_of_nonterm; non_term <= int_end_of_nonterm; ++non_term)
		{
			for (size_t production_index = 0; production_index < production_table[non_term].size(); ++production_index)
			{
				for (size_t pos_B = 0; pos_B < production_table[non_term][production_index].size(); ++pos_B)
				{
					//if(ret[0].size()==2)
					//	__debugbreak();
					set_follow_of_X_with_certain_beta(
						changed, first_table, ret, production_table,
						epsilon, (T)non_term,
						production_index, pos_B);
				}
			}
		}
	}
	return ret;
}

template<typename T>
typename SetHelper<T>::LL1_Table SetHelper<T>::Preanalysis(
	const First_Table& first_table,
	const Follow_Table& follow_table,
	const Production_Table& production_table,
	const T epsilon, const T end_symbol, const T first_nonterm)
{
	auto int_first_nonterm = (int)first_nonterm;
	auto int_end_nonterm = (int)epsilon - 1;
	auto int_epsilon = (int)epsilon;
	LL1_Table ret(int_end_nonterm + 1, std::vector<Production>(end_symbol - epsilon));
	for (size_t nonterm = int_first_nonterm; nonterm < production_table.size(); ++nonterm)
	{
		//A
		for (const auto& production : production_table[nonterm])
		{
			bool has_epsilon = false;
			//A -> ¦Á
			//¦Á -> Y1Y2...Yk
			//FIRST(¦Á)
			for (size_t sym_index = 0; sym_index < production.size(); ++sym_index)
			{
				//for every symbol Yj
				auto Yj = production[sym_index];
				auto firstset_of_Yj = first_table[Yj];
				for (auto term : firstset_of_Yj)
				{
					has_epsilon = false;
					if (term == int_epsilon)
					{
						has_epsilon = true;
						continue;
					}
					auto column = term - epsilon - 1;
					ret[nonterm][column] = production;
				}
				//once a Yj doesn't have int_epsilon , stop.  
				if (!has_epsilon)
					break;
			}
			//if ¦Å belongs to FIRST(¦Á)
			if (has_epsilon)
				for (auto term : follow_table[nonterm])
				{
					auto column = term - epsilon - 1;
					ret[nonterm][column] = production;
				}
		}
	}
	return ret;
}

template<typename T>
typename SetHelper<T>::SimplifiedSetOfItems SetHelper<T>::CLOSURE(
	const Production_Table& production_table,
	const std::set<Item> cores,
	const T& epsilon, const T& start)
{
	SimplifiedSetOfItems ret(cores, epsilon - start);
	for (const auto& core_item : ret.cores)
	{
		// for every core_item
		const auto& production =
			production_table[core_item.nonterm][core_item.production_index];
		// if A -> ¦Á.a¦Â, discard.
		if (core_item.point_pos >= production.size()
			|| production[core_item.point_pos] >= epsilon)
			continue;
		// else:
		// A->¦Á.B¦Â
		auto B = production[core_item.point_pos];
		// B->¦Ã
		ret.non_cores[B] = true;
	}

	auto changed = true;
	while (changed)
	{
		changed = false;
		// A->¦Á.B¦Â
		for (size_t B = 0; B < ret.non_cores.size(); ++B)
			if (ret.non_cores[B])// if has CLOSURE(B)
				for (const auto& production : production_table[B])
					if (//if B-> C¦Ã, where C is nonterm (e.g. E -> T)
						production[0] < (int)epsilon
						//And doesn't contain CLOSURE(C)
						&& !ret.non_cores[production[0]])
					{
						ret.non_cores[production[0]] = true;
						changed = true;
					}
	}
	return ret;
}

template<typename T>
typename SetHelper<T>::SimplifiedSetOfItems SetHelper<T>::GOTO(
	const Production_Table& production_table,
	const SimplifiedSetOfItems& I,
	const T& symbol, const T& epsilon, const T& start)
{
	std::set<Item> cores;
	//for core_item
	for (const auto& core_item : I.cores)
	{
		// for every core_item
		const auto& production =
			production_table[core_item.nonterm][core_item.production_index];
		if (core_item.point_pos < production.size()
			// e.g. E->E.+T symbol = +
			&& production[core_item.point_pos] == symbol
			// e.g A->.¦Å
			&& symbol != epsilon)
		{
			//e.g.  insert E->E+.T
			//insert Item(initialized by list)
			cores.insert({ (T)core_item.nonterm,core_item.production_index,core_item.point_pos + 1 });
		}

	}
	//for non_core_item
	for (size_t nonterm = 0; nonterm < I.non_cores.size(); ++nonterm)
	{
		if (I.non_cores[nonterm])// if has CLOSURE(nonterm)
			for (int production_index = 0;
				production_index < production_table[nonterm].size();
				++production_index)
				if (// e.g. E->.T
					production_table[nonterm][production_index][0] == symbol
					// e.g. A->.¦Å
					&& symbol != epsilon)
				{
					// e.g.  insert E->T.
					// insert Item(initialized by list)
					cores.insert({ (T)nonterm,production_index,1 });
				}
	}
	SimplifiedSetOfItems ret = CLOSURE(production_table, cores, epsilon, start);
	return ret;
}

template<typename T>
std::tuple<typename SetHelper<T>::CollectionOfItemSets, typename SetHelper<T>::Goto_Table>
SetHelper<T>::COLLECTION(const Production_Table& production_table, const T& epsilon, const T& end, const T& start)
{
	std::tuple<CollectionOfItemSets, Goto_Table> ret;
	CollectionOfItemSets& collection = std::get<0>(ret);
	Goto_Table& table = std::get<1>(ret);

	SimplifiedSetOfItems I0 = CLOSURE(
		production_table,
		std::set<Item>({ {start,0,0} }),
		epsilon, start);
	collection.push_back(I0);

	// e.g. Process:
	// generation	1st		2nd		3rd	
	//				I0		I1		I4
	//						I2		I5
	//						I3
	// certain Ii must be producted by Im, who is exactly
	// one generation early than Ii. 
	// and every thing producted by Ii should be checked,
	// whether this state already exists in all privous
	// generations. i.e. whether we need to generate a new
	// Item in the bottom of the Collection. 
	bool changed = true;
	size_t iter = 0;
	size_t start_pos = 0;
	while (changed)
	{
		// in order to record where to start in next loop 
		iter = collection.size();
		changed = false;
		for (size_t i = start_pos; i < collection.size(); ++i)
			for (int sym = 0; sym <= (int)end; ++sym)
			{
				// for every symbol X
				SimplifiedSetOfItems temp_I =
					GOTO(production_table, collection[i], (T)sym, epsilon, start);
				bool inexist = true;
				if (temp_I.cores.size())
					for (int j = 0; j < collection.size(); ++j)
					{
						if (temp_I.my_equals(collection[j]))
						{
							inexist = false;
							// GOTO[state_i,symbol] -> state_j
							table[{i, (T)sym}] = j;
							break;
						}
					}
				else
				{
					continue;
				}

				if (inexist)
				{
					// generate a new state which is now the last elem of collection
					table[{i, (T)sym}] = collection.size();
					collection.push_back(temp_I);
					changed = true;
				}
			}
		start_pos = iter;
	}
	return ret;
}

template<typename T>
typename SetHelper<T>::Action_Table SetHelper<T>::SetActionTable(
	const Production_Table& production_table,
	const CollectionOfItemSets& collection,
	const Goto_Table& goto_table,
	const Follow_Table& follow_table,
	const T& epsilon, const T& end, const T& start)
{
	Action_Table ret;
	auto int_end = (int)end;
	auto int_epsilon = (int)epsilon;
	auto int_start = (int)start;
	for (const auto& item : goto_table)
	{		
		auto sym = std::get<1>(std::get<0>(item));
		auto int_sym = (int)sym;
		auto cur_state = std::get<0>(std::get<0>(item));
		auto aim_state = std::get<1>(item);
		// every certain goto in goto_table
		if (int_sym >= int_epsilon && int_sym < int_end)
			// if the state is A->¦Á.a¦Â, a is a term
		{
			// then ACTION[i,a] = move in a
			Action a;
			a.type = ActionType::move_in;
			// also record the next state to be moved into the stack
			a.aim_state = aim_state;
			a.production_length = 0;
			ret[{cur_state, sym}] = std::move(a);
		}
	}
	for (size_t i = 0; i < collection.size(); ++i)
	{
		// every certain Ii
		for (const Item& core : collection[i].cores)
		{
			// every certain core
			auto production_length = production_table[(int)core.nonterm][core.production_index].size();
			if (core.point_pos >= production_length)
				// if the state is A->¦Á.
			{
				if (core.nonterm == start)
					// if S'-> S.
				{
					// then ACTION[i,$] = accept
					Action a;
					a.type = ActionType::accept;
					a.aim_state = 0;
					a.production_length = production_length;
					ret[{i, end}] = std::move(a);
				}
				else
					for (const auto& term : follow_table[(int)core.nonterm])
					{
						// then for every term a, in FOLLOW(A)
						// ACTION[i,a] = reduce A->¦Á
						Action a;
						a.type = ActionType::reduce;
						// also record the head of the production
						a.nonterm = core.nonterm;
						a.production_length = production_length;
						ret[{i, term}] = std::move(a);
					}
			}
		}
	}
	return ret;
}

template<typename T>
void SetHelper<T>::setup_first_table(
	bool& changed,
	First_Table& first_table,
	const Production_Table& production_table,
	const T epsilon, const T sym)
{
	auto int_sym = (int)sym;
	auto int_epsilon = (int)epsilon;
	//for X -> a
	if (int_sym >= int_epsilon)
	{
		if (first_table[sym].insert(sym).second)
			changed = true;
	}
	else
	{
		//productions of X:
		//X -> alpha | beta | ...
		for (size_t production_index = 0; production_index < production_table[int_sym].size(); ++production_index)
		{
			// a certain production: X -> Y1Y2...Yk
			if (production_table[int_sym][production_index][0] != int_epsilon)
				//if the production is not X -> epsilon
			{
				bool has_epsilon = false;
				for (size_t nonterm_index = 0; nonterm_index < production_table[int_sym][production_index].size(); ++nonterm_index)
				{
					//for every symbol Yj
					auto Yj = production_table[int_sym][production_index][nonterm_index];
					auto firstset_of_Yj = first_table[Yj];
					for (auto term : firstset_of_Yj)
					{
						has_epsilon = false;
						if (term == int_epsilon)
						{
							has_epsilon = true;
							continue;
						}
						if (first_table[int_sym].insert(term).second) changed = true;
					}
					//once a Yj doesn't have int_epsilon , stop.  
					if (!has_epsilon)
						break;
				}
				//if all Yj has int_epsilon , then put int_epsilon into FIRST(X);
				if (has_epsilon && first_table[int_sym].insert(epsilon).second)
					changed = true;
			}
			//when the production is X -> epsilon
			else if (first_table[int_sym].insert(epsilon).second)
				changed = true;
		}
	}
}

template<typename T>
void SetHelper<T>::set_follow_of_X_with_certain_beta(
	bool& changed,
	const First_Table& first_table,
	Follow_Table& follow_table,
	const Production_Table& production_table,
	const T epsilon, const T sym_A,
	const int production_index, const int pos_B)
{
	auto int_sym_A = (int)sym_A;
	auto int_sym_B = (int)production_table[int_sym_A][production_index][pos_B];
	const size_t beta_start_index = pos_B + 1;
	// if(int_sym_A == 1 && beta_start_index == 2)
	//	__debugbreak();

	// if B is not non-term 
	if (int_sym_B >= (int)epsilon)
		return;

	// if A -> ¦ÁB
	// then all FOLLOW(A) belongs to FOLLOW(B)
	if (beta_start_index >= production_table[int_sym_A][production_index].size())
	{
		// then all FOLLOW(A) belongs to FOLLOW(B)
		const auto& follow_of_A = follow_table[int_sym_A];
		for (auto term : follow_of_A)
		{
			if (follow_table[int_sym_B].insert(term).second)
				changed = true;
		}
		return;
	}

	// A -> ¦ÁB¦Â		
	// all FIRST(¦Â) (excepts ¦Å) belongs to FIRST(B)
	bool has_epsilon = false;
	const size_t length_of_production = production_table[int_sym_A][production_index].size();
	// ¦Â = X1X2...Xm
	for (size_t index_in_beta = beta_start_index; index_in_beta < length_of_production; ++index_in_beta)
	{
		// for certain Xi
		T sym_X = production_table[int_sym_A][production_index][index_in_beta];
		int int_sym_X = (int)sym_X;
		for (auto term : first_table[int_sym_X])
		{
			if (term == epsilon)
			{
				has_epsilon = true;
				continue;
			}
			if (follow_table[int_sym_B].insert(term).second)
				changed = true;
		}
		if (!has_epsilon)
			break;
	}

	// X -> ¦ÁB¦Â	
	// if FIRST(¦Â) has ¦Å
	// then all FOLLOW(A) belongs to FOLLOW(B)
	if (has_epsilon)
	{
		const auto& follow_of_A = follow_table[int_sym_A];
		for (auto term : follow_of_A)
		{
			if (follow_table[int_sym_B].insert(term).second)
				changed = true;
		}
	}
}