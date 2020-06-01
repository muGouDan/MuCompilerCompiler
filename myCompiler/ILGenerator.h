#pragma once
#include "SyntaxDirected.h"
#include "ILSymbolTable.h"
class ILGenerator:public SyntaxDirected<ILGenerator>
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
	struct PackedToken
	{
		const Token* token;
		PackedToken(const Token* token_ptr):token(token_ptr){}
	};
	ILEnv Env;
	//M0		->	epsilon
	static void* begin_def(INPUT)
	{
		_this->Env.SetVarTable();
		return nullptr;
	}
	//M1		->	epsilon
	static void* begin_typedef(INPUT)
	{
		_this->Env.SetTypeTable();
		return nullptr;
	}
	//TyDef		->	TyHead { M2 Defs } ;
	static void* end_typedef(INPUT)
	{	
		auto entry = GetPtr(ILEntry, 0);
		entry->width = _this->Env.offset;
		_this->Env.PopTable();
		return nullptr;
	}
	//TyHead	->	Record Id;	
	static void* set_typeHead(INPUT)
	{
		auto entry = _this->Env.CreateILEntry();
		entry->meta_type = GetValue(PackedToken, 0).token->name;
		entry->token = GetValue(PackedToken, 1).token;
		_this->Env.AddEntryToCurrent(entry);
		//head of type_table doesn't care about offset 
		entry->offset = 0;
		return entry;
	}
	//M2		->	epsilon
	static void* new_table(INPUT)
	{
		_this->Env.PushAndNewTable();
		return nullptr;
	}
	//Def		->	Ty Id ;
	static void* set_as_def(INPUT)
	{
		auto entry = GetPtr(ILEntry, 0);
		auto token = GetPtr(PackedToken, 1)->token;
		entry->token = token;
		_this->Env.AddEntryToCurrent(entry);
		return nullptr;
	}
	//Ty		->	CTy
	//Ty		->	BTy
	static void* passon_type(INPUT)
	{
		return PassOn(0);
	}
	//Ty		->	Ty Arr;	
	static void* complete_arrayType(INPUT)
	{
		auto entry = GetPtr(ILEntry, 0);
		const auto& array_info = GetValue(std::vector<size_t>, 1);
		entry->array_info = array_info;
		for (size_t size : array_info)
			entry->width *= size;
		return entry;
	}
	//Arr		->	Arr Cmp
	static void* building_array(INPUT)
	{
		auto array_info = GetPtr(std::vector<size_t>,0);
		auto token = GetPtr(PackedToken, 1)->token;
		array_info->push_back(std::stoi(token->name));
		return array_info;
	}
	//Arr		->	Cmp
	static void* begin_array(INPUT)
	{
		auto array_info = Create(std::vector<size_t>);
		auto token = GetPtr(PackedToken, 0)->token;
		array_info->push_back(std::stoi(token->name));
		return array_info;
	}
	//CTy		->	id;
	static void* set_customeType(INPUT)
	{
		ILEntry* entry = nullptr;
		const auto& token = CurrentToken;
		for (const auto& type : *_this->Env.type_head)
			if (type->token->name == token.name)
				entry = _this->Env.CreateVarFromCType(type);
		return entry;
	}
	//BTy		->	bTy;
	static void* set_baseType(INPUT)
	{
		auto entry = _this->Env.CreateILEntry();
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
	static void* set_component(INPUT)
	{
		return PassOn(1);
	}	
	//Record	->	record
	//Num		->	num
	//Id		->	id
	static void* get_token(INPUT)
	{
		return Create(PackedToken, &CurrentToken);
	}
};

