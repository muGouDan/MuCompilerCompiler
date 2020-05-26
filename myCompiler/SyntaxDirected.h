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

#define INPUT						Type* ptr, std::vector<void*> input, size_t token_iter, const Token_Set& token_set
#define TokenSet					token_set
#define TokenIter					token_iter
#define CurrentToken				token_set[token_iter]

#define This						ptr
#define AddAction(action)			AddSemanticAction(#action,action) 
#define Initialization				FinishSemanticActionTable()
#define GetValue(Type,index)		(*(Type*)(input[index]))		
#define PassOn(index)				input[index]
#define Create(Type,...)			MakeStorage(ptr,Type(__VA_ARGS__))
#define CreateAs(Type,expr)			MakeStorage<Type>(ptr,expr)
#define CreateFrom(expr)			MakeStorage(ptr,expr)
//#define SYNTAX_ACTION_DEBUG
//#define HIGH_LIGHT

struct Sealed
{
	void* ptr = nullptr;
	virtual ~Sealed() {}
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

template<typename T>
class SyntaxDirected
{
protected:
	using Type = T;
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
	std::vector<std::vector<void*(*)(Type* ptr, std::vector<void*>,size_t,const Token_Set&)>> quick_semantic_action_table;
	static void* No_Action(Type*, std::vector<void*>, size_t, const Token_Set&)
	{
		return nullptr;
	}
	std::map<std::string, void*(*)(Type* ptr, std::vector<void*>, size_t, const Token_Set&)> initial_semantic_action_table;
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
		return ptr->quick_semantic_action_table[nonterm][pro_index](ptr->derive_ptr,input,token_iter, *ptr->token_set);
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

	void AddSemanticAction(const char* statement_label, void* (action)(Type* ptr, std::vector<void*>, size_t, const Token_Set&))
	{
		initial_semantic_action_table[statement_label] = action;
	}

private:
	Type* derive_ptr;
#pragma endregion

};

template<typename T>
size_t SyntaxDirected<T>::syntax_unique_id = 0;

template<typename T>
void SyntaxDirected<T>::SyntaxAction_SetHead(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
{
	switch (nonterm)
	{
	case Head:
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[Head]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
		if (!ptr->record.count(ptr->token_set_for_production[token_iter].name))
		{
			ptr->sym_table.push_back(ptr->token_set_for_production[token_iter].name);
			ptr->record.insert({ ptr->token_set_for_production[token_iter].name,ptr->sym_table.size() - 1 });
		}
		break;
	case Pro:
		if (pro_index == 0)
#ifdef SYNTAX_ACTION_DEBUG
			std::cout << "[Pro -> Head axis Body]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
		break;
	default:
		break;
	}
}

template<typename T>
void SyntaxDirected<T>::SyntaxAction_SetBody(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
{
	switch (nonterm)
	{
	case Head:
		ptr->current_flag.head = ptr->record[ptr->token_set_for_production[token_iter].name];
		break;
	case IDs:
		if (pro_index == 1)// IDs->ss_sym
		{
			ptr->current_flag.start = token_iter;
			ptr->current_flag.production_length = 1;
#ifdef SYNTAX_ACTION_DEBUG
			std::cout << "[IDs -> ss_sym]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
		}
		else
		{
			++ptr->current_flag.production_length;
#ifdef SYNTAX_ACTION_DEBUG
			std::cout << "[IDs -> IDs ss_sym]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
		}
		if (!ptr->record.count(ptr->token_set_for_production[token_iter].name))
		{
			ptr->sym_table.push_back(ptr->token_set_for_production[token_iter].name);
			ptr->record.insert({ ptr->token_set_for_production[token_iter].name,ptr->sym_table.size() - 1 });
		}
		break;
	case Body:
	{
		Production production;
		for (size_t i = ptr->current_flag.start;
			i < ptr->current_flag.start + ptr->current_flag.production_length; ++i)
			production.push_back(ptr->record[ptr->token_set_for_production[i].name]);
		ptr->production_table[ptr->current_flag.head].push_back(std::move(production));
		ptr->quick_semantic_action_table[ptr->current_flag.head].push_back(No_Action);
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[Body -> IDs ;]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
	}
	break;
	case ActionLabel:
		ptr->statements[ptr->token_set_for_production[token_iter].name] =
		{
			ptr->current_flag.head,
			ptr->production_table[ptr->current_flag.head].size() - 1
		};
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[ActionLabel]:\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
		break;
	default:
		break;
	}
}

template<typename T>
typename SyntaxDirected<T>::Sym SyntaxDirected<T>::TransferForDefinition(SyntaxDirected* ptr, const Scanner::Token& token)
{
	for (const auto& candidate : ptr->quick_candidates)
	{
		if (token.type == candidate.type)
			return candidate.sym;
		else if (token.name != "" && token.name == candidate.name)
			return candidate.sym;
	}
	if (token.type == Scanner::TokenType::end_symbol)
		return ptr->end;
	throw(std::logic_error("TransferForDefinition Failed"));
}

template<typename T>
SyntaxDirected<T>::SyntaxDirected(std::string path) :
	syntax_production_table(
		{
			//Whole_ -> Whole
			{{Whole}},
			//Whole -> Whole Pro | Pro
			{{Whole,Pro},{Pro}},
			//Pro -> Head "->" Body | Head "->" Body Statement
			{{Head,axis,Body},{Head,axis,Body,Statement}},
			//Head -> ss_sym
			{{ss_sym}},
			//Body -> IDs;
			{{IDs,semi}},
			//IDs -> IDs ss_sym | ss_sym
			{{IDs,ss_sym},{ss_sym}},
			//Statement -> { ss_sym };
			{{le,ActionLabel,re,semi}},
			//Content
			{{ss_sym}}
		})
{
	derive_ptr = (Type*)this;
	slr_parser_for_syntax.SetUp(syntax_production_table, (Syntax_Symbol)(ss_end - 1), ss_end, ss_epsilon, Whole_);
	//slr_parser_for_definition.SetUp(definition_production_table,(Syntax_Symbol)(ss_end-1),ss_end,ss_epsilon,Whole_);
	auto inputs = FileLoader(path, "$$");
	auto& definition_input = inputs[0];
	auto& production_input = inputs[1];
	token_set_for_definition = EasyScanner(definition_input);
	token_set_for_production = EasyScanner(production_input);
#ifdef HIGH_LIGHT
	std::cout << "SyntaxLoader:" << std::endl;
	std::cout << "Definition:" << std::endl;
	Highlight(definition_input, token_set_for_definition);
	std::cout << "Production:" << std::endl;
	Highlight(production_input, token_set_for_production);
#endif // HIGH_LIGHT
	// Setup Candidates
	for (size_t i = 0; i < token_set_for_definition.size() / 4; ++i)
	{
		// if identifier, regard as definition via "TokenType -> terminator"
		if (token_set_for_definition[i * 4 + 2].type == Scanner::TokenType::identifier)
			candidates.push_back({ token_set_for_definition[i * 4 + 2].name,"", token_set_for_definition[i * 4 + 0].name });
		// if raw_string, regard as definition via "TokenName -> terminator"
		else if (token_set_for_definition[i * 4 + 2].type == Scanner::TokenType::raw_string)
			candidates.push_back({ "none",token_set_for_definition[i * 4 + 2].name, token_set_for_definition[i * 4 + 0].name });
		candidate_record.insert(token_set_for_definition[i * 4 + 0].name);
	}

	token_set_for_production.push_back(Scanner::Token{ Scanner::TokenType::end_symbol });
	// Get nonterm
	std::cout << "[First Parse: get nonterm]" << std::endl;
	slr_parser_for_syntax.information = "[First Parse: get nonterm]";
	if (!slr_parser_for_syntax.Parse(token_set_for_production, TransferForSyntax, SyntaxAction_SetHead, this))
		throw std::exception("Syntax Config: Syntax Mistakes, check your syntax config text");

	// set the size of production_table,cause all nonterms have been recognized 
	production_table.resize(sym_table.size());
	quick_semantic_action_table.resize(sym_table.size());
	epsilon = sym_table.size();
	sym_table.push_back("epsilon");

	// Get term
	std::cout << "[Second Parse: get term and semantic action label]" << std::endl;
	slr_parser_for_syntax.information = "[Second Parse: get term and semantic action label]";
	slr_parser_for_syntax.Parse(token_set_for_production, TransferForSyntax, SyntaxAction_SetBody, this);
	end = sym_table.size();
	sym_table.push_back("$");

	// Set Quick Candidates with the help of Candidates
	for (const auto& candidate : candidates)
	{
		auto sym = record[candidate.sym_name];
		Scanner::TokenType type = Scanner::TokenType::none;
		SELECT_TOKENTYPE(candidate.type, type, none)
			SELECT_TOKENTYPE(candidate.type, type, rel_op)
			SELECT_TOKENTYPE(candidate.type, type, log_op)
			SELECT_TOKENTYPE(candidate.type, type, arith_op)
			SELECT_TOKENTYPE(candidate.type, type, keyword)
			SELECT_TOKENTYPE(candidate.type, type, identifier)
			SELECT_TOKENTYPE(candidate.type, type, digit)
			SELECT_TOKENTYPE(candidate.type, type, assign)
			SELECT_TOKENTYPE(candidate.type, type, separator)
			SELECT_TOKENTYPE(candidate.type, type, raw_string)
			SELECT_TOKENTYPE(candidate.type, type, end_symbol);
		quick_candidates.push_back({ type,candidate.name,sym });
	}

	// Set rest Candidates
	for (const auto& sym : record)
		if (sym.second > epsilon && !candidate_record.count(sym.first))
			quick_candidates.push_back({ Scanner::TokenType::none,sym.first,sym.second });
}