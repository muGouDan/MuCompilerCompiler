#pragma once
#include "SyntaxDirected.h"
#include "ILSymbolTable.h"
#include "LR1Parser.h"
class ILGenerator:public SyntaxDirected<ILGenerator,LR1>
{
public:
	ILGenerator(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialization();
	}
	virtual void SetupSemanticActionTable() override
	{
		AddAction(begin_def);
		AddAction(begin_typedef);
		AddAction(end_typedef);
		AddAction(set_typeHead);
		AddAction(new_table);
		AddAction(set_as_def);
		AddAction(passon_type);
		AddAction(complete_arrayType);
		AddAction(building_array);
		AddAction(begin_array);
		AddAction(set_component);
		AddAction(set_baseType);
		AddAction(set_customeType);
		AddAction(get_token);
	}
	void ShowTables()
	{
		std::cout << "TypeTable:" << std::endl;
		for (auto entry : *Env.type_head)
			std::cout << *entry << std::endl;
		std::cout << "VarTable:" << std::endl;
		for (auto entry : *Env.var_head)
			std::cout << *entry << std::endl;
	}
private:
	ILEnv Env;
	//M0		->	epsilon
	void* begin_def(INPUT)
	{
		Env.SetVarTable();
		return nullptr;
	}
	//M1		->	epsilon
	void* begin_typedef(INPUT)
	{
		Env.SetTypeTable();
		TokenSet[TokenIter+1].color = enmCFC_Cyan;
		return nullptr;
	}
	//TyDef		->	TyHead { M2 Defs } ;
	void* end_typedef(INPUT)
	{	
		auto entry = GetPtr(ILEntry, 0);
		for (size_t i = TokenIter + 1; i < TokenSet.size(); ++i)
			if (TokenSet[i].name == entry->token->name)
			{
				TokenSet[i].type = Scanner::TokenType::custom_type;
				TokenSet[i].color = enmCFC_Cyan;
			}			 
		entry->width = Env.offset;
		Env.PopTable();
		return nullptr;
	}
	//TyHead	->	Record Id;	
	void* set_typeHead(INPUT)
	{
		auto entry = Env.CreateILEntry();
		entry->meta_type = GetPtr(Scanner::Token, 0)->name;
		entry->token = GetPtr(Scanner::Token, 1);
		Env.AddEntryToCurrent(entry);
		//head of type_table doesn't care about offset 
		entry->offset = 0;
		return entry;
	}
	//M2		->	epsilon
	void* new_table(INPUT)
	{
		Env.PushAndNewTable();
		return nullptr;
	}
	//Def		->	Ty Id ;
	void* set_as_def(INPUT)
	{
		auto entry = GetPtr(ILEntry, 0);
		auto token = GetPtr(Scanner::Token, 1);
		entry->token = token;
		Env.AddEntryToCurrent(entry);
		return nullptr;
	}
	//Ty		->	CTy
	//Ty		->	BTy
	void* passon_type(INPUT)
	{
		return PassOn(0);
	}
	//Ty		->	Ty Arr;	
	void* complete_arrayType(INPUT)
	{
		auto entry = GetPtr(ILEntry, 0);
		const auto& array_info = GetValue(std::vector<size_t>, 1);
		entry->array_info = array_info;
		for (size_t size : array_info)
			entry->width *= size;
		return entry;
	}
	//Arr		->	Arr Cmp
	void* building_array(INPUT)
	{
		auto array_info = GetPtr(std::vector<size_t>,0);
		auto token = GetPtr(Scanner::Token, 1);
		array_info->push_back(std::stoi(token->name));
		return array_info;
	}
	//Arr		->	Cmp
	void* begin_array(INPUT)
	{
		auto array_info = Create(std::vector<size_t>);
		auto token = GetPtr(Scanner::Token, 0);
		array_info->push_back(std::stoi(token->name));
		return array_info;
	}
	//CTy		->	id;
	void* set_customeType(INPUT)
	{
		ILEntry* entry = nullptr;
		const auto& token = CurrentToken;
		for (const auto& type : *Env.type_head)
			if (type->token->name == token.name)
				entry = Env.CreateVarFromCType(type);
		return entry;
	}
	//BTy		->	bTy;
	void* set_baseType(INPUT)
	{
		auto entry = Env.CreateILEntry();
		const auto& token = CurrentToken;
		entry->meta_type = token.name;
		if (token.name == "int")
			entry->width = ILType::Width::INT;
		else if (token.name == "float")
			entry->width = ILType::Width::FLOAT;
		else if (token.name == "char")
			entry->width = ILType::Width::CHAR;
		return entry;
	}
	//Cmp		->	[ Num ]
	void* set_component(INPUT)
	{
		return PassOn(1);
	}	
	//Record	->	record
	//Num		->	num
	//Id		->	id
	void* get_token(INPUT)
	{
		return &CurrentToken;
	}
};

