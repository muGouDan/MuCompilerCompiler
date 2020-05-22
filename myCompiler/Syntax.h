#pragma once
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include "FileLoader.h"
#include "Scanner.h"
#include "Highlight.h"
#include "SLRParser.h"
#define SELECT_TOKENTYPE(token_type_name,type,element) if(token_type_name==#element) type = Scanner::TokenType::element; else
#define END_SELECT ;
class SyntaxLoader
{
	using Token_Set = std::vector<Scanner::Token>;
	using Sym = size_t;
	// A -> ¦Á
	using Production = std::vector<Sym>;
	// A -> ¦Á0|¦Á1|...
	using Productions = std::vector<Production>;
	// A -> ¦Á0|¦Á1|...
	// B -> ¦Â0|¦Â2|...
	// ...
	using Production_Table = std::vector<Productions>;

	//===========
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
	static void SyntaxAction_SetHead(SyntaxLoader* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
	{
		switch (nonterm)
		{
		case Head:
			std::cout << "Head: " << ptr->token_set_for_production[token_iter] << std::endl;
			if (!ptr->record.count(ptr->token_set_for_production[token_iter].name))
			{
				ptr->sym_table.push_back(ptr->token_set_for_production[token_iter].name);
				ptr->record.insert({ ptr->token_set_for_production[token_iter].name,ptr->sym_table.size() - 1 });
			}
			break;
		case Pro:
			if(pro_index == 0)
				std::cout << "Pro -> Head axis Body " << ptr->token_set_for_production[token_iter] << std::endl;
			break;
		default:
			break;
		}
	}
	//Get Body
	static void SyntaxAction_SetBody(SyntaxLoader* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
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
				std::cout << "[IDs -> ss_sym]" << ptr->token_set_for_production[token_iter] << std::endl;
			}			
			else
			{
				++ptr->current_flag.production_length;
				std::cout << "[IDs -> IDs ss_sym]" << ptr->token_set_for_production[token_iter] << std::endl;
			}	
			if (!ptr->record.count(ptr->token_set_for_production[token_iter].name))
			{
				ptr->sym_table.push_back(ptr->token_set_for_production[token_iter].name);
				ptr->record.insert({ ptr->token_set_for_production[token_iter].name,ptr->sym_table.size() - 1 });
			}
			break;
		case Body:
				ptr->flags.push_back(ptr->current_flag);
				std::cout << "[Body -> IDs ;]" << ptr->token_set_for_production[token_iter] << std::endl;
			break;
		default:
			break;
		}
	}
	

	//SLRParser<Syntax_Symbol> slr_parser_for_definition;
	//std::vector<std::vector<std::vector<Syntax_Symbol>>> definition_production_table;
	//===========

	Sym start = 0;
	Sym epsilon = 0;
	Sym end = 0;
	Token_Set token_set_for_production;
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
	Production_Table production_table;
	
	Token_Set token_set_for_definition;

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
	static Sym TransferForDefinition(SyntaxLoader* ptr, const Scanner::Token& token)
	{
		for (const auto& candidate : ptr->quick_candidates)
		{
			if (token.type == candidate.type)
				return candidate.sym;
			else if (token.name!=""&&token.name == candidate.name)
				return candidate.sym;
		}
		if (token.type == Scanner::TokenType::end_symbol)
			return ptr->end;
		throw(std::logic_error("TransferForDefinition Failed"));
	}
public:
	SyntaxLoader(std::string path) :
		syntax_production_table(
			{
				//Whole_
				{{Whole}},
				//Whole
				{{Whole,Pro},{Pro}},
				//Pro
				{{Head,axis,Body},{Head,axis,Body,Statement}},
				//Head
				{{ss_sym}},
				//Body
				{{IDs,semi}},
				//IDs
				{{IDs,ss_sym},{ss_sym}},
				//Statement
				{{le,ss_sym,re,semi}},
			}
			)
		/*
		definition_production_table(
			{
				//Whole_
				{{Whole}},
				//Whole
				{{Whole,Pro},{Pro}},
				//Pro
				{{Head,axis,Body}},
				//Head
				{{ss_sym}},
				//Body
				{{IDs,semi}},
				//IDs
				{{IDs,ss_sym},{ss_sym}},
				//Statement
				{{le,ss_sym,re,semi}},
			}
		)*/
	{
		slr_parser_for_syntax.SetUp(syntax_production_table,(Syntax_Symbol)(ss_end-1),ss_end,ss_epsilon,Whole_);
		//slr_parser_for_definition.SetUp(definition_production_table,(Syntax_Symbol)(ss_end-1),ss_end,ss_epsilon,Whole_);
		auto inputs = FileLoader("my_syntax.cfg","$$");
		auto& definition_input = inputs[0];
		auto& production_input = inputs[1];
		token_set_for_definition = EasyScanner(definition_input);
		token_set_for_production = EasyScanner(production_input);
		std::cout << "SyntaxLoader:" << std::endl;
		std::cout << "Definition:" << std::endl;
		Highlight(definition_input, token_set_for_definition);
		std::cout << "Production:" << std::endl;
		Highlight(production_input, token_set_for_production);
		
		// Setup Candidates
		for (size_t i = 0; i < token_set_for_definition.size()/4; ++i)
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
		slr_parser_for_syntax.Parse(token_set_for_production, TransferForSyntax, SyntaxAction_SetHead, this);
		// set the size of production_table,cause all nonterms have been recognized 
		production_table.resize(sym_table.size());
		epsilon = sym_table.size();
		sym_table.push_back("epsilon");

		// Get term
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
		{
			if (sym.second > epsilon && !candidate_record.count(sym.first))
			{
				quick_candidates.push_back({ Scanner::TokenType::none,sym.first,sym.second });
			}
		}


		// Setup production table
		for (const auto& flag:flags)
		{
			Production production;
			for (size_t i = flag.start; i < flag.start + flag.production_length; ++i)
			{
				production.push_back(record[token_set_for_production[i].name]);
			}	
			production_table[flag.head].push_back(std::move(production));
		}

		slr_parser.SetUp(production_table, end - 1, end, epsilon, start);
	}

	void Parse(const Token_Set& token_set)
	{
		slr_parser.Parse(token_set, TransferForDefinition, this);
	}
};

size_t SyntaxLoader::syntax_unique_id = 0;