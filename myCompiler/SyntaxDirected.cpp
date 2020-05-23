#pragma once
#include "SyntaxDirected.h"
size_t SyntaxDirected::syntax_unique_id = 0;

void SyntaxDirected::SyntaxAction_SetHead(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
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

void SyntaxDirected::SyntaxAction_SetBody(SyntaxDirected* ptr, size_t nonterm, size_t pro_index, size_t token_iter)
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

SyntaxDirected::Sym SyntaxDirected::TransferForDefinition(SyntaxDirected* ptr, const Scanner::Token& token)
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

SyntaxDirected::SyntaxDirected(std::string path) :
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

	slr_parser_for_syntax.SetUp(syntax_production_table, (Syntax_Symbol)(ss_end - 1), ss_end, ss_epsilon, Whole_);
	//slr_parser_for_definition.SetUp(definition_production_table,(Syntax_Symbol)(ss_end-1),ss_end,ss_epsilon,Whole_);
	auto inputs = FileLoader(path, "$$");
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

