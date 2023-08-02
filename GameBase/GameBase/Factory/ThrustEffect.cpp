#include "ThrustEffect.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/ThrusterBehavior.h"
#include "../Component/Render/EffectRender.h"
#include "../Component/Behavior/EffectBehavior.h"

ThrustEffect::ThrustEffect(ObjectManager& objectManager) :
    Factory{objectManager}
{
}

ObjectID ThrustEffect::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();

	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetAttribute(ObjectAttribute::Other);
	info->SetUseFactory(false);
	objectManager_.AddComponent(std::move(info), id);

	auto behavior = objectManager_.GetPool().Pop<EffectBehavior>();
	behavior->SetFactoryID(GetID());
	objectManager_.AddComponent(std::move(behavior), id);

	auto render = objectManager_.GetPool().Pop<EffectRender>();
	static_cast<Render&>(*render).Load(L"Resource/Effect/Thrust.efkefc");

	// テストでわかりやすいようにビームにしてる
	//static_cast<Render&>(*render).Load(L"Resource/Effect/Beem.efkefc");
	objectManager_.AddComponent(std::move(render), id);

	//auto parent = objectManager_.GetComponent<Transform>(ownerID);
	//parent->AddChild(id);
	auto trans = objectManager_.GetPool().Pop<Transform>();
	//trans->SetParent(ownerID);
	trans->SetRotation({ rot.x,rot.y,rot.z });
	trans->Pos() = pos;
	objectManager_.AddComponent(std::move(trans), id);

	return id;
}
