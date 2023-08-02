#include "PlayerAttackFactory.h"
#include "../Object/ObjectManager.h"
#include "../Component/ComponentPool.h"
#include "../Component/Behavior/PlayerAttackBehavior.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Collider/CapsuleCollider.h"

#include "../Common/Debug.h"

PlayerAttackFactory::PlayerAttackFactory(ObjectManager& objectManager) :
	Factory{ objectManager }
{
}

ObjectID PlayerAttackFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{

	auto id = objectManager_.MakeObjectID();
	
	DebugLog("プレイヤー攻撃生成ID=", *id);

	// 動き
	auto behavior = objectManager_.GetPool().Pop<PlayerAttackBehavior>();
	//behavior->SetAttackTime(2.0f);
	objectManager_.AddComponent(std::move(behavior), id);

	// オブジェクトの情報
	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetUseFactory(true);
	info->SetAttribute(ObjectAttribute::PlayerAttack);
	objectManager_.AddComponent(std::move(info), id);

	// トランスフォーム
	auto transform = objectManager_.GetPool().Pop<Transform>();
	transform->Pos() = zeroVector3<float>;
	// transform->SetParent(ownerID);
	transform->AddRotation({ Deg2Rad(90.0f), 0.0f, 0.0f });
	objectManager_.AddComponent(std::move(transform), id);

	// 当たり判定
	auto col = objectManager_.GetPool().Pop<CapsuleCollider>();
	col->SetOffset({ 0.0,-60.0f,0.0f });
	col->SetRadius(30.0f);
	col->SetHeight(35.0f);

	objectManager_.AddComponent(std::move(col), id);

	objectManager_.SetPlayerAttackID(id);

	return id;
}

void PlayerAttackFactory::Destroy(ObjectID id)
{
	auto& pool = objectManager_.GetPool();
	pool.Push(objectManager_.RemoveComponent<PlayerAttackBehavior>(id));
	pool.Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	pool.Push(objectManager_.RemoveComponent<Transform>(id));
	pool.Push(objectManager_.RemoveComponent<CapsuleCollider>(id));
}
