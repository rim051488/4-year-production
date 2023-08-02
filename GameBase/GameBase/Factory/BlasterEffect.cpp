#include "BlasterEffect.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/ThrusterBehavior.h"
#include "../Component/Render/EffectRender.h"

BlasterEffect::BlasterEffect(ObjectManager& objectManager) :
	Factory{objectManager}
{
}

ObjectID BlasterEffect::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetAttribute(ObjectAttribute::Other);
	info->SetUseFactory(true);
	objectManager_.AddComponent(std::move(info), id);


	auto behavior = objectManager_.GetPool().Pop<ThrusterBehavior>();
	//behavior->SetFactoryID(GetID());
	objectManager_.AddComponent(std::move(behavior), id);


	auto modelRender = objectManager_.GetPool().Pop<EffectRender>();
	static_cast<Render&>(*modelRender).Load("Resource/Effect/Blaster.efkefc");
	objectManager_.AddComponent(std::move(modelRender), id);

	/*auto player = objectManager_.GetComponent<Transform>(ownerID);
	player->AddChild(id);*/
	auto trans = objectManager_.GetPool().Pop<Transform>();
	//trans->SetParent(ownerID);
	trans->Pos() = pos;

	objectManager_.AddComponent(std::move(trans), id);

	return id;
}

void BlasterEffect::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ThrusterBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<EffectRender>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
}
