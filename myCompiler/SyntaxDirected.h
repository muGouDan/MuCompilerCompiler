#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include "FileLoader.h"
#include "Scanner.h"
#include "Highlight.h"
#include "SLRParser.h"
//#define SEMANTIC_CHECK
//#define SYNTAX_ACTION_DEBUG
//#define SHOW_CATCHED_VAR
//#define HIGH_LIGHT


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
		LabeledPro,
		Pro,
		Head,
		Body,
		IDs,
		Statement,
		ActionLabel,
		ScopeLabel,

		ss_epsilon,
		ss_sym,
		axis, //->
		le,//{ 
		re,//}
		semi,//;
		colon,//:
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
			else if (token.name == ":") return colon;
			else return ss_sym;
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
#pragma endregion
	Sym start = 0;
	Sym epsilon = 0;
	Sym end = 0;
	Token_Set token_set_for_production;
	Token_Set token_set_for_definition;
	SLRParser<Sym> slr_parser;
	std::map<std::string, Sym> record;
	std::set<std::string> heads;
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
	std::string scope_label = "Untitled";
	size_t part = 0;
	std::string ScopedName(std::string name)
	{
		if (name != "epsilon")
			return scope_label + "." + name;
		else
			return name;
	}
	std::string ScopedName(const Scanner::Token& token)
	{
		if (part == 0) return token.name;
		std::string name = scope_label + "." + token.name;
		if (part == 2 && !heads.count(name))
		{
#ifdef SEMANTIC_CHECK
			if (token.name[0] >= 'A' && token.name[0] <= 'Z')
				std::cout << "[Semantic Check]: \n[" << token <<
				"] starts with big letter,but is recognized as term" << std::endl;
#endif // CHECK_ON
			return token.name;
		}
		return name;
		//return token.name;
	}
	struct InfoPair
	{
		Sym nonterm;
		size_t pro_index;
	};
	std::multimap<std::string, InfoPair> statements;
	// find statement with (nonterm,pro_index)
	std::vector<std::vector<void* (*)(Type* ptr, std::vector<void*>, size_t, const Token_Set&)>> quick_semantic_action_table;
	static void* No_Action(Type*, std::vector<void*>, size_t, const Token_Set&)
	{
		return nullptr;
	}
	std::map<std::string, void* (*)(Type* ptr, std::vector<void*>, size_t, const Token_Set&)> initial_semantic_action_table;
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
	Candidate candidate_flag;
	static Sym TransferForDefinition(SyntaxDirected* ptr, const Scanner::Token& token);
	static void SyntaxAction_SetUpDefinition(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter);
	static void SyntaxAction_SetHead(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter);
	static void SyntaxAction_SetBody(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter);
	static void* SemanticActionDispatcher(
		SyntaxDirected* ptr, std::vector<void*> input,
		size_t nonterm, size_t pro_index, size_t token_iter)
	{
		return ptr->quick_semantic_action_table[nonterm][pro_index](ptr->derive_ptr, input, token_iter, *ptr->token_set);
	}
public:
	SyntaxDirected(std::string path);

	bool Parse(const Token_Set& token_set)
	{
		this->token_set = &token_set;
		return slr_parser.Parse(token_set, TransferForDefinition, SemanticActionDispatcher, this);
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

class TestCompiler :public SyntaxDirected<TestCompiler>
{
public:
	TestCompiler(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialization;
	}
};

template<typename T>
size_t SyntaxDirected<T>::syntax_unique_id = 0;

template<typename T>
void SyntaxDirected<T>::SyntaxAction_SetHead(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
{
	switch (nonterm)
	{
	case Head:
	{
#ifdef SEMANTIC_CHECK
		if (ptr->token_set_for_production[token_iter].name[0] >= 'a' && ptr->token_set_for_production[token_iter].name[0] <= 'z')
			std::cout << "[Semantic Check]:\n[" << ptr->token_set_for_production[token_iter] <<
			"] starts with small letter,but is recognized as nonterm" << std::endl;
#endif // CHECK_ON
		std::string name = ptr->ScopedName(ptr->token_set_for_production[token_iter]);
		if (!ptr->record.count(name))
		{
			ptr->sym_table.push_back(name);
			ptr->heads.insert(name);
			ptr->record.insert({ name,ptr->sym_table.size() - 1 });
		}
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[Head]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
	}
	break;
	case Pro:
#ifdef SYNTAX_ACTION_DEBUG
		if (pro_index == 0)
			std::cout << "[Pro -> Head axis Body]\n" << ptr->token_set_for_production[token_iter] << std::endl;
		else
			std::cout << "[Pro -> Head axis Body Statement]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
		break;
	case ScopeLabel:
		ptr->scope_label = ptr->token_set_for_production[token_iter-1].name;
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[ScopeLabel -> ss_sym:]\n" << ptr->token_set_for_production[token_iter - 1] << std::endl;
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
	{
		std::string name = ptr->ScopedName(ptr->token_set_for_production[token_iter]);
		ptr->current_flag.head = ptr->record[name];
	}	
	break;
	case IDs:
	{
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
		// if identifier, add the scope label
		std::string name = ptr->ScopedName(ptr->token_set_for_production[token_iter]);
		if (!ptr->record.count(name))
		{
			if (ptr->heads.count(name))
				//if is Head(nonterm)
			{
				ptr->sym_table.push_back(name);
				ptr->record.insert({ name,ptr->sym_table.size() - 1 });
			}
			else
				//if is term
			{
				ptr->sym_table.push_back(
					ptr->token_set_for_production[token_iter].name);
				ptr->record.insert({ ptr->token_set_for_production[token_iter].name,ptr->sym_table.size() - 1 });
			}
		}
	}
	break;
	case Body:
	{
		Production production;
		for (size_t i = ptr->current_flag.start;
			i < ptr->current_flag.start + ptr->current_flag.production_length; ++i)
			production.push_back(ptr->record[ptr->ScopedName(ptr->token_set_for_production[i])]);
		ptr->production_table[ptr->current_flag.head].push_back(std::move(production));
		ptr->quick_semantic_action_table[ptr->current_flag.head].push_back(No_Action);
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[Body -> IDs ;]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
	}
	break;
	case ActionLabel:
	{
		std::string name = ptr->token_set_for_production[token_iter].name;
		ptr->statements.insert(
		{
			name,
			{	
				ptr->current_flag.head,
				ptr->production_table[ptr->current_flag.head].size() - 1
			}
		});

	}
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[ActionLabel]:\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
	break;
	case ScopeLabel:
		ptr->scope_label = ptr->token_set_for_production[token_iter-1].name;
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[ScopeLabel -> ss_sym:]\n" << ptr->token_set_for_production[token_iter - 1] << std::endl;
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
void SyntaxDirected<T>::SyntaxAction_SetUpDefinition(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
{
	switch (nonterm)
	{
	case Head:
#ifdef SEMANTIC_CHECK
		if (ptr->token_set_for_definition[token_iter].name[0] >= 'A'
			&& ptr->token_set_for_definition[token_iter].name[0] <= 'Z')
			std::cout <<  "[Semantic Check]:\n[" << ptr->token_set_for_definition[token_iter] <<
			"] starts with big letter,but is recognized as term" << std::endl;
#endif // CHECK_ON
		ptr->candidate_flag.sym_name = ptr->token_set_for_definition[token_iter].name;
		break;
	case IDs:
		if (pro_index == 1)// IDs->ss_sym
		{
			// if identifier, regard as definition via "TokenType -> terminator"
			if (ptr->token_set_for_definition[token_iter].type == Scanner::TokenType::identifier)
			{
				ptr->candidate_flag.type = ptr->token_set_for_definition[token_iter].name;
				ptr->candidate_flag.name = "";
			}
			// if raw_string, regard as definition via "TokenName -> terminator"
			else if (ptr->token_set_for_definition[token_iter].type == Scanner::TokenType::raw_string)
			{
				ptr->candidate_flag.name = ptr->token_set_for_definition[token_iter].name;
				ptr->candidate_flag.type = "none";
			}
			ptr->candidates.push_back(ptr->candidate_flag);
			ptr->candidate_record.insert(ptr->candidate_flag.sym_name);
#ifdef SYNTAX_ACTION_DEBUG
			std::cout << "[IDs -> ss_sym]\n" << ptr->token_set_for_production[token_iter] << std::endl;
#endif
			}
		else
			throw(std::logic_error("Too Much IDs"));
		break;
	case ScopeLabel:
		ptr->scope_label = ptr->token_set_for_definition[token_iter-1].name;
#ifdef SYNTAX_ACTION_DEBUG
		std::cout << "[ScopeLabel -> ss_sym:]\n" << ptr->token_set_for_definition[token_iter-1] << std::endl;
#endif
		break;
	default:
		break;
		}
	}

template<typename T>
SyntaxDirected<T>::SyntaxDirected(std::string path) :
	syntax_production_table(
		{
			//Whole_ -> Whole
			{{Whole}},
			//Whole -> Whole LabeledPro | LabeledPro
			{{Whole,LabeledPro},{LabeledPro}},
			//LabeledPro -> ScopeLabel Pro
			{{ScopeLabel,Pro},{Pro}},
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
			//ActionLabel
			{{ss_sym}},
			//ScopeLabel -> ss_sym:
			{{ss_sym,colon}}
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
	token_set_for_production.push_back(Scanner::Token{ Scanner::TokenType::end_symbol });
	token_set_for_definition.push_back(Scanner::Token{ Scanner::TokenType::end_symbol });

	std::string type_name = typeid(Type).name();
	std::cout << "[Syntax Parse:<" << type_name << ">] START" << std::endl;

	slr_parser_for_syntax.information = "[First Part Parse:<" + type_name + ">]";
	part = 0;
	// Setup Candidates
	if (!slr_parser_for_syntax.Parse(token_set_for_definition, TransferForSyntax, SyntaxAction_SetUpDefinition, this))
		throw std::exception("Syntax Config: Syntax Mistakes, check the first part of your syntax config text");
#ifdef SHOW_CATCHED_VAR
	std::cout << "Catched Candidates(term):" << std::endl;
	for (const auto& candidate : candidates)
		std::cout << candidate.sym_name << " -> "
		<< "[type: " << candidate.type << "]"
		"[name:\" " << candidate.name << "\"]" << std::endl;
#endif // SHOW_CATCHED_VAR
	
	slr_parser_for_syntax.information = "[Second Part Parse:<" + type_name + "> Head]";
	part = 1;
	// Get nonterm
	scope_label = "Untitled";
	if (!slr_parser_for_syntax.Parse(token_set_for_production, TransferForSyntax, SyntaxAction_SetHead, this))
		throw std::exception("Syntax Config: Syntax Mistakes, check the second part of your syntax config text");

	// set the size of production_table,cause all nonterms have been recognized 
	production_table.resize(sym_table.size());
	quick_semantic_action_table.resize(sym_table.size());
	epsilon = sym_table.size();
	record.insert({ "epsilon",sym_table.size() });
	sym_table.push_back("epsilon");

#ifdef SHOW_CATCHED_VAR
	std::cout << "Catched Heads(nonterm):" << std::endl;
	for (size_t i = 0; i < sym_table.size() - 1; ++i)
	{
		std::cout << i << " : " << sym_table[i] << std::endl;
	}	
#endif // SHOW_CATCHED_VAR

	slr_parser_for_syntax.information = "[Second Part Parse:<" + type_name + "> Body]";
	part = 2;
	// Get term
	scope_label = "Untitled";
	slr_parser_for_syntax.Parse(token_set_for_production, TransferForSyntax, SyntaxAction_SetBody, this);
	end = sym_table.size();
	sym_table.push_back("$");

#ifdef SHOW_CATCHED_VAR
	std::cout << "Generated Symbols:" << std::endl;
	for (size_t i = 0; i < sym_table.size(); ++i)
	{
		std::cout << i << " : " << sym_table[i] << std::endl;
	}
#endif // SHOW_CATCHED_VAR
#ifdef SHOW_CATCHED_VAR
	std::cout << "Catched ActionNames:" << std::endl;
	for (const auto& item : statements)
		std::cout << item.first << " : " << sym_table[item.second.nonterm] 
		<< " -> Index[" << item.second.pro_index <<"]" << std::endl;
#endif // SHOW_CATCHED_VAR
	
	std::cout << "[Syntax Parse:<" << type_name << ">] SUCCEED" << std::endl;
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
		{
			quick_candidates.push_back({ Scanner::TokenType::none, sym.first,sym.second });
		}
#ifdef SHOW_CATCHED_VAR
	std::cout << "Generated Candidates(nonterm):" << std::endl;
	std::cout << "Info: if your nonterm has been catched as candidate" << std::endl;
	std::cout << "that may because your nonterm doesn't have any ProductionBody" << std::endl;
	for (const auto& candidate : quick_candidates)
		std::cout <<"\""<<sym_table[candidate.sym] << "\"" 
		<<"("<< candidate.sym <<")" << " -> "
		"[name:\" " << candidate.name << "\"]" << std::endl;
#endif // SHOW_CATCHED_VAR
}