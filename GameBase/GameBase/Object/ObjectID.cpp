#include "ObjectID.h"

std::hash<long long> ObjectID::hash_;

ObjectID::ObjectID()
{
	id_ = 0ull;
}

ObjectID::ObjectID(long long seed)
{
	id_ = hash_(seed);
}


ObjectID::ObjectID(const ObjectID& objID) :
	id_{objID.id_}
{
}

ObjectID::ObjectID(const size_t& val) :
	id_{ val }
{
}

ObjectID& ObjectID::operator=(const ObjectID& objID)
{
	id_ = objID.id_;
	return *this;
}

bool ObjectID::operator==(const ObjectID& objID)
{
	return id_ == objID.id_;
}

const size_t ObjectID::operator*() const
{
	return id_;
}
