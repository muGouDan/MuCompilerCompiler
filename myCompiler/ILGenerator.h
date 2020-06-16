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
		AddAction(check_member);
		AddAction(check_var_in_table);
		AddAction(complete_memId);
		AddAction(complete_array);
		AddAction(create_array_dimension);
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
private:
	ILEnv Env;
	std::stack<size_t> dimension_stack;
	size_t cur_dim = 0;
	void CreateAndPushDim()
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
	//LValCmp	->	LValCmp "." LValMem
	void* check_member(INPUT)
	{
		auto& base_entry = GetValue(ILEntry, 0);
		auto& sub_entry = GetValue(ILEntry, 2);
		bool name_equal = false;
		bool dimension_equal = false;
		std::stringstream ss;
		for (auto type_entry : *Env.type_head)
			if (type_entry->token->name == base_entry.meta_type)
				for (auto entry : *type_entry->table_ptr)
				{
					name_equal = entry->token->name == sub_entry.token->name;
					dimension_equal = entry->array_info.size() == sub_entry.array_info.size();
					if (name_equal)
					{
						if (dimension_equal)
							return entry;
						else
						{
							ss << "ERROR:array[" << entry->token->name
								<< "]'s dimension is " << entry->array_info.size()
								<< ",but this[" << sub_entry.token->name
								<< "]'s dimension is " << sub_entry.array_info.size();
						}
					}
				}
		if(!name_equal)
			ss << "ERROR: identifier[" << sub_entry.token->name
				<< "] is undefined";
		Highlight(*this->input, TokenSet, TokenIter, ss.str());
		return SEMANTIC_ERROR;
	}
	//LValCmp -> LValMem
	void* check_var_in_table(INPUT)
	{
		auto& temp_entry = GetValue(ILEntry, 0);
		bool name_equal = false;
		bool dimension_equal = false;
		std::stringstream ss;
		for (auto entry : *Env.top)
		{
			name_equal = entry->token->name == temp_entry.token->name;
			dimension_equal = entry->array_info.size() == temp_entry.array_info.size();
			if (name_equal)
			{
				if (dimension_equal)
					return entry;
				else
				{
					ss	<< "ERROR:array[" << entry->token->name
						<< "]'s dimension is " << entry->array_info.size()
						<< ",but this[" << temp_entry.token->name
						<< "]'s dimension is " << temp_entry.array_info.size();
				}
			}
		}
		if (!name_equal)
			ss << "ERROR: identifier[" << temp_entry.token->name
			<< "] is undefined";
		Highlight(*this->input, TokenSet, TokenIter, ss.str());
		return SEMANTIC_ERROR;
	}
	//MemId	->	Id
	void* complete_memId(INPUT)
	{
		auto token = GetPtr(Token, 0);
		auto temp_entry = Env.CreateILEntry();
		temp_entry->token = token;	
		return temp_entry;
	}
	//ArrVar ->	Id M_Ar Arr;
	void* complete_array(INPUT)
	{
		auto token = GetPtr(Token, 0);
		auto temp_entry = Env.CreateILEntry();
		temp_entry->token = token;
		temp_entry->array_info.resize(cur_dim);
		PopDim();
		return temp_entry;
	}
	//M_Ar	-> epsilon
	void* create_array_dimension(INPUT)
	{
		CreateAndPushDim();
		return nullptr;
	}
	//Cmp	->	"[" RVal "]"
	void* inc_array_dimension(INPUT)
	{
		++cur_dim;
		return nullptr;
	}
#pragma endregion
};

