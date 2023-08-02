#include "SandSmokeSlowMoveEffect.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/SandSmokeSlowMoveBehavior.h"
#include "../Component/Render/EffectRender.h"

SandSmokeSlowMoveEffect::SandSmokeSlowMoveEffect(ObjectManager& objectManager) :
	Factory{ objectManager }
{
}

ObjectID SandSmokeSlowMoveEffect::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetAttribute(ObjectAttribute::Other);
	info->SetUseFactory(true);
	objectManager_.AddComponent(std::move(info), id);


	auto behavior = objectManager_.GetPool().Pop<SandSmokeSlowMoveBehavior>();
	//behavior->SetFactoryID(GetID());
	objectManager_.AddComponent(std::move(behavior), id);


	auto modelRender = objectManager_.GetPool().Pop<EffectRender>();
	static_cast<Render&>(*modelRender).Load("Resource/Effect/SandSmokeSlowMove.efkefc");
	objectManager_.AddComponent(std::move(modelRender), id);

	auto trans = objectManager_.GetPool().Pop<Transform>();
	trans->Pos() = pos;
	trans->SetRotation({ rot.x,rot.y,rot.z });
	objectManager_.AddComponent(std::move(trans), id);

	return id;
}

void SandSmokeSlowMoveEffect::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<SandSmokeSlowMoveBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<EffectRender>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
}