#pragma once
#include <vector>
#include <iostream>
#include <map>
#include <set>
#include <cassert>
template <typename T>
class SetHelper
{
public:
	using Production = std::vector<T>;
	using Production_Table = std::vector<std::vector<Production>>;
	using First_Table = std::vector<std::set<T>>;
	using Follow_Table = std::vector<std::set<T>>;
	// SELECT(A -> ¦Á) = {a1,a2,...}
	// Select_Table[nonterm][production] => set of term;
	using Select_Table = std::vector<std::map<Production&, std::set<T>>>;
	// LL1_Table[nonterm][term] => Production
	using LL1_Table = std::vector<std::vector<Production>>;
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
		const T epsilon, const T last, const T first = 0)
	{
		auto int_last = (int)last;
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
		const T epsilon, const T end_symbol, const T first_nonterm = (T)0)
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
				for (int production_index = 0; production_index < production_table[non_term].size(); ++production_index)
				{
					for (int pos_B = 0; pos_B < production_table[non_term][production_index].size(); ++pos_B)
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

private:
	//term > epsilon > nonterm
	static void setup_first_table(
		bool& changed,
		First_Table& first_table,
		const Production_Table& production_table,
		const T epsilon, const T sym);

	//for certain A -> ¦ÁB¦Â
	//beta_start_index>=length of production X->¦ÁB¦Â,means X -> ¦ÁB
	static void set_follow_of_X_with_certain_beta(
		bool& changed,
		const First_Table& first_table,
		Follow_Table& follow_table,
		const Production_Table& production_table,
		const T epsilon, const T sym_A,
		const int production_index, const int pos_B);
};

template<typename T>
typename SetHelper<T>::LL1_Table SetHelper<T>::Preanalysis(const First_Table& first_table, const Follow_Table& follow_table, const Production_Table& production_table, const T epsilon, const T end_symbol, const T first_nonterm)
{
	auto int_first_nonterm = (int)first_nonterm;
	auto int_end_nonterm = (int)epsilon - 1;
	auto int_epsilon = (int)epsilon;
	LL1_Table ret(int_end_nonterm + 1, std::vector<Production>(end_symbol - epsilon));
	for (int nonterm = int_first_nonterm; nonterm < production_table.size(); ++nonterm)
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
		if (first_table[sym].count(sym) == 0)
		{
			first_table[sym].insert(sym);
			changed = true;
		}
	}
	else
	{
		//productions of X:
		//X -> alpha | beta | ...
		for (size_t production_index = 0; production_index < production_table[int_sym].size(); ++production_index)
		{
			// a certain production: X -> Y1Y2...Yk
			if (production_table[int_sym][production_index][0] != int_epsilon)
				//if the production is not X -> int_epsilon
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
						if (first_table[int_sym].count(term) == 0)
						{
							first_table[int_sym].insert(term);
							changed = true;
						}
					}
					//once a Yj doesn't have int_epsilon , stop.  
					if (!has_epsilon)
						break;
				}
				if (has_epsilon)//if all Yj has int_epsilon , then put int_epsilon into FIRST(X);
				{
					if (first_table[int_sym].count(epsilon) == 0)
					{
						first_table[int_sym].insert(epsilon);
						changed = true;
					}
				}
			}
			else
				//if the production is X -> int_epsilon
			{
				if (first_table[int_sym].count(epsilon) == 0)
				{
					first_table[int_sym].insert(epsilon);
					changed = true;
				}
			}
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
	const int beta_start_index = pos_B + 1;
	//if(int_sym_A == 1 && beta_start_index == 2)
	//	__debugbreak();

	// if B is not non-term 
	if (int_sym_B >= (int)epsilon)
		return;

	// if A -> ¦ÁB
	//then all FOLLOW(A) belongs to FOLLOW(B)
	if (beta_start_index >= production_table[int_sym_A][production_index].size())
	{
		//then all FOLLOW(A) belongs to FOLLOW(B)
		const auto& follow_of_A = follow_table[int_sym_A];
		for (auto term : follow_of_A)
		{
			if (follow_table[int_sym_B].count(term) == 0)
			{
				follow_table[int_sym_B].insert(term);
				changed = true;
			}
		}
		return;
	}

	// A -> ¦ÁB¦Â		
	//all FIRST(¦Â) (excepts ¦Å) belongs to FIRST(B)
	bool has_epsilon = false;
	const auto length_of_production = production_table[int_sym_A][production_index].size();
	// ¦Â = X1X2...Xm
	for (int index_in_beta = beta_start_index; index_in_beta < length_of_production; ++index_in_beta)
	{
		//for certain Xi
		T sym_X = production_table[int_sym_A][production_index][index_in_beta];
		int int_sym_X = (int)sym_X;
		for (auto term : first_table[int_sym_X])
		{
			if (term == epsilon)
			{
				has_epsilon = true;
				continue;
			}
			if (follow_table[int_sym_B].count(term) == 0)
			{
				follow_table[int_sym_B].insert(term);
				changed = true;
			}
		}
		if (!has_epsilon)
			break;
	}

	//X -> ¦ÁB¦Â	
	//if FIRST(¦Â) has ¦Å
	//then all FOLLOW(A) belongs to FOLLOW(B)
	if (has_epsilon)
	{
		const auto& follow_of_A = follow_table[int_sym_A];
		for (auto term : follow_of_A)
		{
			if (follow_table[int_sym_B].count(term) == 0)
			{
				follow_table[int_sym_B].insert(term);
				changed = true;
			}
		}
	}
}


namespace Example
{
	enum symbol
	{
		E,
		E_,
		T,
		T_,
		F,
		epsilon,
		//term		column
		mul,		//*			0
		plus,		//+			1
		lp,			//(			2
		rp,			//)			3
		id,			//			4
		end			//$			5
	};

	SetHelper<symbol>::Production_Table pro =
	{
		//E
		{
			{T,E_}			//E -> TE'
		},
		//E_
		{
			{plus,T,E_},	//E' -> +TE'
			{epsilon}		//E' -> epsilon
		},
		//T
		{
			{F,T_}			//T -> FT'
		},
		//T_
		{
			{mul,F,T_},		//T' -> *FT'
			{epsilon}		//T' -> epsilon
		},
		//F
		{
			{lp,E,rp},		//F -> (E)
			{id}			//F -> id
		}

	};
}

