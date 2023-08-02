#include "PlayerSkillFactory.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Behavior/PlayerSkillBehavior.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Collider/CapsuleCollider.h"
#include "../Component/Render/EffectRender.h"

PlayerSkillFactory::PlayerSkillFactory(ObjectManager& objectManager) :
	Factory{ objectManager }
{
}

ObjectID PlayerSkillFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();

	// 動き
	auto behavior = objectManager_.GetPool().Pop<PlayerSkillBehavior>();
	behavior->SetAttackTime(3.0f);
	objectManager_.AddComponent(std::move(behavior), id);

	// オブジェクトの情報
	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetUseFactory(true);
	info->SetAttribute(ObjectAttribute::PlayerSkill);
	objectManager_.AddComponent(std::move(info), id);

	// トランスフォーム
	auto transform = objectManager_.GetPool().Pop<Transform>();
	transform->Pos() = zeroVector3<float>;
	transform->SetRotation({ Deg2Rad(90.0f),0.0f,-rot.y });
	objectManager_.AddComponent(std::move(transform), id);

	// 当たり判定
	auto col = objectManager_.GetPool().Pop<CapsuleCollider>();
	col->SetOffset({ 0.0,-2100.0f,-50.0f });
	col->SetRadius(200.0f);
	col->SetHeight(2000.0f);

	objectManager_.AddComponent(std::move(col), id);
	return id;
}

void PlayerSkillFactory::Destroy(ObjectID id)
{
	auto& pool = objectManager_.GetPool();
	pool.Push(objectManager_.RemoveComponent<PlayerSkillBehavior>(id));
	pool.Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	pool.Push(objectManager_.RemoveComponent<Transform>(id));
	pool.Push(objectManager_.RemoveComponent<CapsuleCollider>(id));
}
