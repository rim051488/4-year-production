#include "EnemyAttackFactory.h"
#include "../Component/Behavior/EnemyBehavior.h"
#include "../Component/Behavior/EnemyAttackBehavior.h"
#include "../Component/ComponentPool.h"
#include "../Object/ObjectManager.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Collider/CapsuleCollider.h"

EnemyAttackFactory::EnemyAttackFactory(ObjectManager& objectManager) :
	Factory{objectManager}
{
}

EnemyAttackFactory::~EnemyAttackFactory()
{
}

ObjectID EnemyAttackFactory::Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot)
{
	auto id = objectManager_.MakeObjectID();

	// 動き
	auto behavior = objectManager_.GetPool().Pop<EnemyAttackBehavior>();
	objectManager_.AddComponent(std::move(behavior), id);

	// オブジェクトの情報
	auto info = objectManager_.GetPool().Pop<ObjectInfo>();
	info->SetUseFactory(true);
	info->SetAttribute(ObjectAttribute::EnemyAttack);
	objectManager_.AddComponent(std::move(info), id);

	// トランスフォーム
	auto transform = objectManager_.GetPool().Pop<Transform>();
	transform->Pos() = pos;
	objectManager_.AddComponent(std::move(transform), id);

	// 当たり判定
	auto col = objectManager_.GetPool().Pop<CapsuleCollider>();
	col->SetOffset({ 0.0,70.0f,0.0f });
	col->SetRadius(30.0f);
	col->SetHeight(60.0f);

	objectManager_.AddComponent(std::move(col), id);

	return id;
}

void EnemyAttackFactory::Destroy(ObjectID id)
{
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<EnemyAttackBehavior>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<ObjectInfo>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<Transform>(id));
	objectManager_.GetPool().Push(objectManager_.RemoveComponent<CapsuleCollider>(id));
}
