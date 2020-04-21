#pragma once
#include <string>
#include <vector>
#include <fstream>

std::vector<std::string> FileLoader(std::string path)
{
	std::vector<std::string> lines;
	std::ifstream fs(path);
	char container[1024];
	while (fs.getline(container, 1024))
	{
		lines.push_back(std::string(container));
	}
	fs.close();
	return lines;
}