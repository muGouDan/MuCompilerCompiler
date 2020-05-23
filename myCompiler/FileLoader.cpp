#include "FileLoader.h"

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

std::vector<std::vector<std::string>> FileLoader(std::string path, std::string separator)
{
	size_t iter = 0;
	std::vector<std::vector<std::string>> inputs;
	inputs.push_back(std::vector<std::string>());
	std::ifstream fs(path);
	char container[1024];
	while (fs.getline(container, 1024))
	{
		std::string str(container);
		if (str == separator)
		{
			inputs.push_back(std::vector<std::string>());
			++iter;
		}
		else
			inputs[iter].push_back(str);
	}
	fs.close();
	return inputs;
}
