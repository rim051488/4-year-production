#pragma once
#include "Factory.h"

// çUåÇîªíËóp
class PlayerAttackFactory :
    public Factory
{
public:
	PlayerAttackFactory(ObjectManager& objectManager);
private:
	ObjectID Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot = { 0.0f,0.0f,0.0f }) final;
	void Destroy(ObjectID id) final;
	const FactoryID GetID(void) const { return FactoryID::PlayerAttack; }
};

