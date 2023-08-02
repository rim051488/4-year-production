#include "VerticalEffect.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/EffectBehavior.h"
#include "../Component/Render/EffectRender.h"

VerticalEffect::VerticalEffect(ObjectManager& objectManager) :
	Factory{objectManager}
{
}

ObjectID VerticalEffect::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{ 
	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetAttribute(ObjectAttribute::Other);
	info->SetUseFactory(true);
	objectManager_.AddComponent(std::move(info), id);

	auto behavior = objectManager_.GetPool().Pop<EffectBehavior>();
	behavior->SetFactoryID(GetID());
	behavior->SetSyncID(ownerID);
	behavior->SetType(EffectBehaviorType::SyncAll);
	objectManager_.AddComponent(std::move(behavior), id);

	auto render = objectManager_.GetPool().Pop<EffectRender>();
	static_cast<Render&>(*render).Load(L"Resource/Effect/Vertical.efkefc");
	objectManager_.AddComponent(std::move(render), id);

	auto trans = objectManager_.GetPool().Pop<Transform>();
	trans->Pos() = pos;
	trans->SetRotation({ rot.x,rot.y,rot.z });
	objectManager_.AddComponent(std::move(trans), id);

	return id;
}

void VerticalEffect::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<EffectBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<EffectRender>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
}
