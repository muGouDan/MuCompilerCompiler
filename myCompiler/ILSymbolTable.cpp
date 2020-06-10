#include "ILSymbolTable.h"

size_t ILEntry::tabs = 0;

std::ostream& operator<<(std::ostream& out, const ILEntry& entry)
{
	ILEntry::ShowAllEntry(&entry);
	return std::cout;
}

ILEntry* ILEnv::CreateVarFromCType(ILEntry* src)
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

ILEntry* ILEnv::Copy(const ILEntry* current)
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

void ILEntry::ShowRawEntry(const ILEntry* to_show)
{
	std::cout << "MetaType<" << to_show->meta_type;
	for (const auto& item : to_show->array_info)
		std::cout << "[" << item << "]";
	std::cout << "> ";
	std::cout << "TokenName[" << to_show->token->name << "] ";
	std::cout << "Width[" << to_show->width << "] ";
	std::cout << "Offset[" << to_show->offset << "] ";
}

void ILEntry::ShowAllEntry(const ILEntry* current)
{
	ShowRawEntry(current);
	if (current->table_ptr && current->independent)
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
