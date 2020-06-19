#pragma once
#include <string>
#include <vector>
#include <stack>
#include "BaseWord.h"
#include "SealedValue.h"
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
	std::string value;
	~ILEntry()
	{
		if (table_ptr&&independent)
			delete table_ptr;
	}
	

	friend class ILEnv;
private:
	ILEntry(){}
	friend std::ostream& operator<<(std::ostream& out, const ILEntry& entry);
	static void ShowRawEntry(const ILEntry* to_show);
	static void ShowAllEntry(const ILEntry* current);
	static size_t tabs;
};


std::ostream& operator << (std::ostream& out, const ILEntry& entry);

struct Address
{
	// if size() == 0 , address is a temp;
	std::vector<const ILEntry*> chain;
	std::vector<Address*> array_index;
	std::string code;
	Scanner::Token* token = nullptr;
	friend class ILEnv;
	friend std::ostream& operator << (std::ostream& out, const Address& addr);
private:
	size_t id;
	Address(){}
};

std::ostream& operator << (std::ostream& out, const Address& addr);

struct ILCode
{
	// res op left right
	Address* res;
	std::string op;
	Address* left;
	Address* right;
};

std::ostream& operator << (std::ostream& out, const ILCode& code);

class ILEnv
{
	using ILSymbolTable = std::vector<ILEntry*>;
	
	std::stack<ILSymbolTable*> table_stack;
	std::stack<size_t> offset_stack;	
	std::vector<ILEntry*> ILEntry_heap;
	std::vector<Address*> address_heap;
	std::stack<Sealed*> sealed_value;
	size_t address_id = 0;
public:
	ILSymbolTable* top;
	ILSymbolTable* var_head;
	ILSymbolTable* type_head;
	bool is_typedef = false;
	size_t offset;
	std::vector<ILCode> ILCodeStream;
	ILEnv():
		var_head(new ILSymbolTable()),
		type_head(new ILSymbolTable()),
		offset(0)
	{
		top = var_head;
	}
	~ILEnv()
	{
		while (!sealed_value.empty())
		{
			delete sealed_value.top();
			sealed_value.pop();
		}
		for (auto entry : ILEntry_heap)
			delete entry;
		for (auto address : address_heap)
			delete address;
	}
	template<typename T>
	T* CreateValue(T obj)
	{
		auto temp = new SealedValue<T>(obj);
		sealed_value.push(temp);
		return temp->value;
	}

	ILEntry* CreateILEntry()
	{
		auto temp = new ILEntry();
		ILEntry_heap.push_back(temp);
		return temp;
	}
	Address* CreateAddress()
	{
		auto temp = new Address();
		temp->id = address_id++;
		address_heap.push_back(temp);
		return temp;
	}
	ILEntry* CreateVarFromCType(ILEntry* src);
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
		entry->offset = ceil((double)offset/(double)entry->width)* entry->width;
		offset += entry->width;
		top->push_back(entry);
	}

	void PushTable()
	{
		table_stack.push(top);
		offset_stack.push(offset);
		offset = 0;
	}

	ILSymbolTable* CreateILTableAsCurrent()
	{
		auto temp = new ILSymbolTable();
		top = temp;
		return temp;
	}

	ILEntry* LastILEntryInCurTable()
	{
		return (*top)[top->size() - 1];
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
	ILEntry* Copy(const ILEntry* current);
};

