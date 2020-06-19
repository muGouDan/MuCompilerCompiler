#pragma once
#include "SyntaxDirected.h"
#include "ILSymbolTable.h"
#include "LR1Parser.h"
class ILGenerator :public SyntaxDirected<ILGenerator, LR1>
{
public:
	ILGenerator(std::string cfg_path) :SyntaxDirected(cfg_path)
	{
		Initialization();
	}
	virtual void SetupSemanticActionTable() override
	{
		AddAction(passon_0);
		AddAction(passon_1);
		AddAction(begin_def);
		AddAction(begin_typedef);
		AddAction(end_typedef);
		AddAction(set_typeHead);
		AddAction(new_table);
		AddAction(set_as_def);
		AddAction(set_def_with_val);
		AddAction(complete_arrayType);
		AddAction(building_array);
		AddAction(begin_array);
		AddAction(set_baseType);
		AddAction(set_customeType);
		AddAction(get_token);

		AddAction(assign);
		AddAction(create_addr);
		AddAction(complete_lVal);
		AddAction(start_lVal);
		AddAction(filling_addr);
		AddAction(start_Cmp);
		AddAction(create_id_addr);
		AddAction(complete_array);
		AddAction(create_array_dimension);
		AddAction(process_offset);
		AddAction(inc_array_dimension);
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
	void ShowILCode()
	{
		for (auto& code : Env.ILCodeStream)
			std::cout << code << std::endl;
	}
private:
	ILEnv Env;
	size_t cur_dim = 0;
	Address* cur_addr = nullptr;
	std::stack<Address*> addr_stack;
	std::stack<size_t> dimension_stack;
	void PushAndCreateDim()
	{
		dimension_stack.push(cur_dim);
		cur_dim = 0;
	}
	void PopDim()
	{
		cur_dim = dimension_stack.top();
		dimension_stack.pop();
	}
#pragma region _Global
	void* passon_0(INPUT)
	{
		return PassOn(0);
	}
	void* passon_1(INPUT)
	{
		return PassOn(1);
	}
	void* get_token(INPUT)
	{
		return &CurrentToken;
	}
#pragma endregion

#pragma region _Definition
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
		TokenSet[TokenIter + 1].color = enmCFC_Cyan;
		return nullptr;
	}
	//TyDef		->	TyHead { M2 Defs }
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
	//TyHead	->	Record Id	
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
		Env.PushTable();
		auto entry = Env.LastILEntryInCurTable();
		entry->table_ptr = Env.CreateILTableAsCurrent();
		return nullptr;
	}
	//Def		->	Ty Id
	void* set_as_def(INPUT)
	{
		auto entry = GetPtr(ILEntry, 0);
		auto token = GetPtr(Scanner::Token, 1);
		entry->token = token;
		Env.AddEntryToCurrent(entry);
		return nullptr;
	}
	//Def		->	Ty Id "=" Id
	void* set_def_with_val(INPUT)
	{
		auto entry = GetPtr(ILEntry, 0);
		auto token = GetPtr(Scanner::Token, 1);
		entry->token = token;
		entry->value = GetValue(Token, 3).name;
		Env.AddEntryToCurrent(entry);
		return nullptr;
	}
	//Ty		->	Ty Arr
	void* complete_arrayType(INPUT)
	{
		constexpr size_t Ty = 0;
		constexpr size_t Arr = 1;
		auto entry = GetPtr(ILEntry, Ty);
		const auto& array_info = GetValue(std::vector<size_t>, Arr);
		entry->array_info = array_info;
		size_t amount = 1;
		size_t width = entry->width;
		for (size_t size : array_info)
		{
			entry->width *= size;
			amount *= size;
		}
		Env.PushTable();
		entry->table_ptr = Env.CreateILTableAsCurrent();
		for (size_t i = 0; i < amount; ++i)
		{
			auto sub_entry = Env.CreateILEntry();
			sub_entry->meta_type = entry->meta_type;
			sub_entry->width = width;
			Env.AddEntryToCurrent(sub_entry);
		}
		Env.PopTable();
		return entry;
	}
	//Arr		->	Arr Cmp
	void* building_array(INPUT)
	{
		auto array_info = GetPtr(std::vector<size_t>, 0);
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
	//CTy		->	id
	void* set_customeType(INPUT)
	{
		ILEntry* entry = nullptr;
		const auto& token = CurrentToken;
		for (const auto& type : *Env.type_head)
			if (type->token->name == token.name)
				entry = Env.CreateVarFromCType(type);
		return entry;
	}
	//BTy		->	bTy
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
#pragma endregion

#pragma region _Assignment:
	//Asgn	->	LVal "=" RVal ";"
	void* assign(INPUT)
	{
		Env.ILCodeStream.push_back({ GetPtr(Address,0),"",GetPtr(Address,2),NIL });
		return NIL;
	}
	//Val		->	ImVal
	void* create_addr(INPUT)
	{
		auto token = GetPtr(Token, 0);
		auto addr = Env.CreateAddress();
		addr->token = token;
		return addr;
	}
	//LVal		->	M0 LValCmp
	void* complete_lVal(INPUT)
	{
		Env.PopTable();
		return PassOn(1);
	}
	//M0		->	epsilon
	void* start_lVal(INPUT)
	{
		Env.PushTable();
		Env.top = Env.var_head;
		return NIL;
	}
	//LValCmp	->	LValCmp "." LValMem
	void* filling_addr(INPUT)
	{
		auto LValCmp = GetPtr(Address, 0);
		auto LValMem = GetPtr(Address, 2);
		LValCmp->chain.push_back(LValMem->chain[0]);
		if (LValMem->array_index.size())
			LValCmp->array_index.push_back(LValMem->array_index[0]);
		return LValCmp;
	}
	//LValCmp	->	LValMem
	void* start_Cmp(INPUT)
	{
		auto addr = GetPtr(Address, 0);
		Env.top = addr->chain[addr->chain.size() - 1]->table_ptr;
		return addr;
	}
	//LValMem	->	MemId
	void* create_id_addr(INPUT)
	{
		auto token = GetPtr(Token, 0);
		if (Env.top != nullptr)
		{
			for (auto entry : *Env.top)
				if (token->name == entry->token->name)
				{
					auto addr = Env.CreateAddress();
					addr->chain.push_back(entry);
					return addr;
				}
		}
		Highlight(*input_text, TokenSet, TokenIter,
			"ERROR: undefined " + token->name);
		return SEMANTIC_ERROR;
	}
	//ArrVar	->	ArrId Arr
	void* complete_array(INPUT)
	{
		auto ArrId = GetPtr(Address, 0);
		auto Arr = GetPtr(Address, 1);
		ArrId->array_index.push_back(Arr);
		cur_addr = addr_stack.top();
		addr_stack.pop();
		return ArrId;
	}
	//ArrId  -> Id
	void* create_array_dimension(INPUT)
	{
		auto token = GetPtr(Token, 0);
		PushAndCreateDim();
		if (Env.top != nullptr)
		{
			for (auto entry : *Env.top)
				if (token->name == entry->token->name)
				{
					auto addr = Env.CreateAddress();
					addr->chain.push_back(entry);
					addr_stack.push(cur_addr);
					cur_addr = addr;
					return addr;
				}
		}
		Highlight(*input_text, TokenSet, TokenIter,
			"ERROR: undefined " + token->name);
		return SEMANTIC_ERROR;
	}
	//Arr		->  Arr Cmp
	void* process_offset(INPUT)
	{
		auto addr1 = GetPtr(Address, 0);
		auto cur = cur_addr->chain[cur_addr->chain.size() - 1];
		auto cur_size = cur->array_info[cur_dim - 1];
		auto addr2 = GetPtr(Address, 1);
		auto temp_mul = Env.CreateAddress();
		auto temp_offset = Env.CreateAddress();
		auto temp_size = Env.CreateAddress();
		temp_size->code = std::to_string(cur_size);
		Env.ILCodeStream.push_back({ temp_mul, "MUL", temp_size, addr1 });
		Env.ILCodeStream.push_back({ temp_offset, "ADD", temp_mul, addr2 });
		return temp_offset;
	}
	//Cmp	->	"[" RVal "]"
	void* inc_array_dimension(INPUT)
	{
		auto addr = GetPtr(Address, 1);
		++cur_dim;
		return addr;
	}
#pragma endregion
};

