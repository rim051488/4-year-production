#pragma once
#include <string>

class ObjectID
{
public:
	ObjectID();
	ObjectID(long long seed);
	ObjectID(const ObjectID& objID);
	ObjectID(const size_t& val);
	ObjectID& operator=(const ObjectID& objID);
	bool operator ==(const ObjectID& objID);
	const size_t operator*()const;
private:
	size_t id_;
	static std::hash<long long> hash_;
};

