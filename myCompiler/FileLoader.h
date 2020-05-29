#pragma once
#include <string>
#include <vector>
#include <fstream>

struct LineContent
{
	std::string content;
	size_t line_no;
};
std::vector<LineContent> FileLoader(std::string path);

std::vector<std::vector<LineContent>> FileLoader(std::string path, std::string separator);