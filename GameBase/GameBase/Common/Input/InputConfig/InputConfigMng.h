#pragma once
#include "../InputID.h"

struct Header
{
	float ver;
	size_t size;
	int sum;
};

struct Data
{
	InputID id;
	int code;
};
