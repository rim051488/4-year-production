#include "BeemEffect.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/BeemEffectBehavior.h"
#include "../Component/Render/EffectRender.h"

BuleBeemEffect::BuleBeemEffect(ObjectManager& objectManager) :
	Factory{objectManager}
{
}

ObjectID BuleBeemEffect::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetAttribute(ObjectAttribute::Other);
	info->SetUseFactory(true);
	objectManager_.AddComponent(std::move(info), id);


	auto behavior = objectManager_.GetPool().Pop<BeemEffectBehavior>();
	objectManager_.AddComponent(std::move(behavior), id);


	auto modelRender = objectManager_.GetPool().Pop<EffectRender>();
	static_cast<Render&>(*modelRender).Load("Resource/Effect/Beem.efkefc");
	objectManager_.AddComponent(std::move(modelRender), id);

	auto trans = objectManager_.GetPool().Pop<Transform>();
	trans->Pos() = pos;
	trans->SetRotation({rot.x, rot.y, rot.z});
	trans->Scale() = Vector3{ 10.0f, 10.0f, 10.0f };

	objectManager_.AddComponent(std::move(trans), id);

	return id;
}

void BuleBeemEffect::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<BeemEffectBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<EffectRender>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
}
