#include "ObjectInfo.h"

ObjectInfo::ObjectInfo() :
	attribute_{ObjectAttribute::Other}
{
}

void ObjectInfo::Begin(ObjectManager& objectManager)
{
	isActive_ = true;
}

void ObjectInfo::End(ObjectManager& objectManager)
{
	isActive_ = false;
	useFactory_ = false;
	attribute_ = ObjectAttribute::Other;
}

const bool ObjectInfo::IsActive(void) const
{
	return isActive_;
}
