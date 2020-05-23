#pragma once
#include <string>
#include <vector>
#include <fstream>

std::vector<std::string> FileLoader(std::string path);

std::vector<std::vector<std::string>> FileLoader(std::string path, std::string separator);