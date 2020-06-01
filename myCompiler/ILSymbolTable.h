#pragma once
#include <string>
#include <vector>
#include "BaseWord.h"
#include <stack>
struct ILType
{
	struct Width
	{
		static const size_t FLOAT = 8;
		static const size_t INT = 4;
		static const size_t CHAR = 1;
	};
	struct Range
	{

	};
};

struct ILEntry
{
	using ILSymbolTable = std::vector<ILEntry*>;
	bool independent = true;
	size_t width = 0;
	size_t offset = 0;
	std::string meta_type;
	std::vector<size_t> array_info;
	const Scanner::Token* token = nullptr;
	ILSymbolTable* table_ptr = nullptr;
	~ILEntry()
	{
		if (table_ptr&&independent)
			delete table_ptr;
	}
	

	friend class ILEnv;
private:
	ILEntry(){}
	friend std::ostream& operator<<(std::ostream& out, const ILEntry& entry);
	static void ShowRawEntry(const ILEntry* to_show)
	{
		std::cout << "MetaType<" << to_show->meta_type;
		for (const auto& item : to_show->array_info)
			std::cout << "[" << item << "]";
		std::cout << "> ";
		std::cout << "TokenName[" << to_show->token->name << "] ";
		std::cout << "Width[" << to_show->width << "] ";
		std::cout << "Offset[" << to_show->offset << "] ";
	}
	static void ShowAllEntry(const ILEntry* current)
	{
		ShowRawEntry(current);
		if (current->table_ptr&&current->independent)
		{	
			++tabs;
			for (size_t i = 0; i < (*current->table_ptr).size(); i++)
			{
				std::cout << std::endl;
				for (size_t j = 0; j < tabs; ++j)
				{					
					if (j != 0)
						std::cout << "©¦";
					std::cout << "\t";
				}
				if (i < (*current->table_ptr).size() - 1)
					std::cout << "©À©¤ ";
				else
					std::cout << "©¸©¤ ";
				ShowAllEntry((*current->table_ptr)[i]);
			}
			--tabs;
		}	
	}
	static size_t tabs;
};


std::ostream& operator << (std::ostream& out, const ILEntry& entry);

class ILEnv
{
	using ILSymbolTable = std::vector<ILEntry*>;
	
	std::stack<ILSymbolTable*> table_stack;
	std::stack<size_t> offset_stack;	
	std::vector<ILEntry*> to_delete;
public:
	ILSymbolTable* top;
	ILSymbolTable* var_head;
	ILSymbolTable* type_head;
	bool is_typedef = false;
	size_t offset;
	ILEnv():
		var_head(new ILSymbolTable()),
		type_head(new ILSymbolTable()),
		offset(0)
	{
		top = var_head;
	}
	~ILEnv()
	{
		for (auto entry : to_delete)
			delete entry;
	}
	ILEntry* CreateILEntry()
	{
		auto temp = new ILEntry();
		to_delete.push_back(temp);
		return temp;
	}
	ILEntry* CreateVarFromCType(ILEntry* src)
	{
		ILEntry* ret = CreateILEntry();
		ret->meta_type = src->token->name;
		ret->width = src->width;
		if (is_typedef)
		{
			ret->table_ptr = src->table_ptr;
			ret->independent = false;
			return ret;
		}
		if (src->table_ptr)
		{
			ret->table_ptr = new ILSymbolTable();
			for (auto entry : *src->table_ptr)
				ret->table_ptr->push_back(Copy(entry));
		}
		return ret;
	}
	void SetVarTable()
	{
		top = var_head;
		is_typedef = false;
	}
	void SetTypeTable()
	{
		top = type_head;
		is_typedef = true;
	}
	// set entry.offset, and change the current offset with entry.width
	// then add the entry to var_head
	void AddEntryToCurrent(ILEntry* entry)
	{
		entry->offset = offset;
		offset += entry->width;
		top->push_back(entry);
	}

	void PushAndNewTable()
	{
		table_stack.push(top);
		auto temp = new ILSymbolTable();
		(*top)[top->size() - 1]->table_ptr = temp;
		top = temp;
		offset_stack.push(offset);
		offset = 0;
	}

	void PopTable()
	{
		if (table_stack.size())
		{
			top = table_stack.top();
			table_stack.pop();
			offset = offset_stack.top();
			offset_stack.pop();
		}
		else
			throw(std::logic_error("Empty Stack"));
	}
private:
	ILEntry* Copy(const ILEntry* current)
	{
		ILEntry* ret = CreateILEntry();
		ret->meta_type = current->meta_type;
		ret->token = current->token;
		ret->offset = current->offset;
		ret->width = current->width;
		ret->array_info = current->array_info;
		if (current->table_ptr)
		{
			ret->table_ptr = new ILSymbolTable();
			for (auto entry : *current->table_ptr)
				ret->table_ptr->push_back(Copy(entry));
		}
		return ret;
	}
};

