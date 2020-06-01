#include "ILSymbolTable.h"

size_t ILEntry::tabs = 0;

std::ostream& operator<<(std::ostream& out, const ILEntry& entry)
{
	ILEntry::ShowAllEntry(&entry);
	return std::cout;
}
