#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "FileLoader.h"
#include "Scanner.h"
#include "Highlight.h"
#include "SLRParser.h"
#define SELECT_TOKENTYPE(token_type_name,type,element) if(token_type_name==#element) type = Scanner::TokenType::element; else
#define END_SELECT ;

#define INPUT						SyntaxDirected* base, std::vector<void*> input, size_t token_iter, const Token_Set& token_set
#define TokenSet					token_set
#define TokenIter					token_iter
#define CurrentToken				token_set[token_iter]
#define Base						base

#define GetThis(type)				dynamic_cast<type*>(base)
#define This						GetThis(ThisType)
#define AddAction(action)			AddSemanticAction(#action,action) 
#define Initialization				FinishSemanticActionTable()
#define GetValue(Type,index)		(*(Type*)(input[index]))		
#define PassOn(index)				input[index]
#define Create(Type,...)			MakeStorage(base,Type(__VA_ARGS__))
#define CreateAs(Type,expr)			MakeStorage<Type>(base,expr)
#define CreateFrom(expr)			MakeStorage(base,expr)
//#define SYNTAX_ACTION_DEBUG
//#define HIGH_LIGHT


class SyntaxDirected
{
public:
	using Token_Set = std::vector<Scanner::Token>;
private:
	using Sym = size_t;
	// A -> ¦Á
	using Production = std::vector<Sym>;
	// A -> ¦Á0|¦Á1|...
	using Productions = std::vector<Production>;
	// A -> ¦Á0|¦Á1|...
	// B -> ¦Â0|¦Â2|...
	// ...
	using Production_Table = std::vector<Productions>;

#pragma region Syntax Rule of Syntax Productions
	enum Syntax_Symbol
	{
		ss_none = -1,
		Whole_,
		Whole,
		Pro,
		Head,
		Body,
		IDs,
		Statement,
		ActionLabel,

		ss_epsilon,
		ss_sym,
		axis, //->
		le,//{ 
		re,//}
		semi,
		ss_end
	};
	SLRParser<Syntax_Symbol> slr_parser_for_syntax;
	std::vector<std::vector<std::vector<Syntax_Symbol>>> syntax_production_table;
	static Syntax_Symbol TransferForSyntax(const Scanner::Token& token)
	{
		if (token.type == Scanner::TokenType::separator)
		{
			if (token.name == "->") return axis;
			else if (token.name == "{") return le;
			else if (token.name == "}") return re;
			else if (token.name == ";") return semi;
			else return ss_none;

		}
		else if (token.type == Scanner::TokenType::end_symbol)
			return ss_end;
		else
			return ss_sym;
	}
	static size_t syntax_unique_id;
	static size_t GetUniqueId()
	{
		return syntax_unique_id++;
	}
	//Get Head
	static void SyntaxAction_SetHead(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter);
	//Get Body
	static void SyntaxAction_SetBody(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter);
#pragma endregion
	Sym start = 0;
	Sym epsilon = 0;
	Sym end = 0;
	Token_Set token_set_for_production;
	Token_Set token_set_for_definition;
	SLRParser<Sym> slr_parser;
	std::map<std::string, Sym> record;
	std::vector<std::string> sym_table;
	struct Production_Flag
	{
		Sym head;
		size_t start;
		size_t production_length = 0;
	};
	std::vector<Production_Flag> flags;
	Production_Flag current_flag;
	const Token_Set* token_set;
	Production_Table production_table;


	struct InfoPair
	{
		Sym nonterm;
		size_t pro_index;
	};
	std::map<std::string,InfoPair> statements;
	// find statement with (nonterm,pro_index)
	std::vector<std::vector<void*(*)(SyntaxDirected* base, std::vector<void*>,size_t,const Token_Set&)>> quick_semantic_action_table;
	static void* No_Action(SyntaxDirected*, std::vector<void*>, size_t, const Token_Set&)
	{
		return nullptr;
	}
	std::map<std::string, void*(*)(SyntaxDirected* base, std::vector<void*>, size_t, const Token_Set&)> initial_semantic_action_table;
	// Candidate and Quick_Candidate are used to tell which Token will be transfered to which terminator
	// e.g. TokenType::identifier -> id || TokenType::digit -> id || Token.name == ")" -> )
	struct Candidate
	{
		std::string type;
		std::string name;
		std::string sym_name;
	};
	struct Quick_Candidate
	{
		Scanner::TokenType type;
		std::string name;
		Sym sym;
	};
	std::set<std::string> candidate_record;
	std::vector<Candidate> candidates;
	std::vector<Quick_Candidate> quick_candidates;
	static Sym TransferForDefinition(SyntaxDirected* ptr, const Scanner::Token& token);
	static void* SemanticActionDispatcher(
		SyntaxDirected* ptr, std::vector<void*> input, 
		size_t nonterm, size_t pro_index, size_t token_iter)
	{
		return ptr->quick_semantic_action_table[nonterm][pro_index](ptr,input,token_iter, *ptr->token_set);
	}
public:
	SyntaxDirected(std::string path);

	bool Parse(const Token_Set& token_set)
	{
		this->token_set = &token_set;
		return slr_parser.Parse(token_set, TransferForDefinition, SemanticActionDispatcher,this);
	}

	~SyntaxDirected()
	{
		while (!to_delete.empty())
		{
			delete to_delete.top();
			to_delete.pop();
		}
	}
#pragma region Semantic Actions
private:
		struct Sealed
		{
			void* ptr = nullptr;
			virtual ~Sealed();
		};

		template<typename T>
		struct SealedValue :public Sealed
		{
			using Type = T;
			SealedValue(T val) :value(new T(val))
			{
				ptr = value;
			}
			T* value;
			virtual ~SealedValue()
			{
				delete value;
			}
		};
protected:
	std::stack<Sealed*> to_delete;

	template<typename T>
	static T* MakeStorage(SyntaxDirected* ptr, T obj)
	{
		auto temp = new SealedValue<T>(obj);
		ptr->to_delete.push(temp);
		return temp->value;
	}

	void FinishSemanticActionTable()
	{
		slr_parser.SetUp(production_table, end - 1, end, epsilon, start);
		SetupSemanticActionTable();
		//set the semantic action from initial_semantic_action_table to quick_semantic_action_table
		for (const auto& statement : statements)
		{
			auto iter = initial_semantic_action_table.find(statement.first);
			if (iter != initial_semantic_action_table.end())
				quick_semantic_action_table[statement.second.nonterm][statement.second.pro_index] = iter->second;
		}
	}

	virtual void SetupSemanticActionTable()
	{
		SetConsoleColor(ConsoleForegroundColor::enmCFC_Yellow, ConsoleBackGroundColor::enmCBC_Blue);
		std::cout << "[Warning]: Haven't Set Semantic Actions!!!" << std::endl;
		SetConsoleColor();
	}

	void AddSemanticAction(const char* statement_label, void* (action)(SyntaxDirected* base, std::vector<void*>, size_t, const Token_Set&))
	{
		initial_semantic_action_table[statement_label] = action;
	}

#pragma endregion

};
