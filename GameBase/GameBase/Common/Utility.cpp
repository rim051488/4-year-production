#include <Windows.h>
#include <cassert>
#include "Utility.h"


std::string ReadString(std::ifstream& file)
{
	int num{ 0 };
	std::string str;
	file.read(reinterpret_cast<char*>(&num), sizeof(num));
	str.resize(num);
	file.read(str.data(), sizeof(str[0]) * num);
	return str;
}

