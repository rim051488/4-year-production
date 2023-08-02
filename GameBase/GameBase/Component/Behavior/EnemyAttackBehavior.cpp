#include "EnemyAttackBehavior.h"
#include <DxLib.h>
#include "../Collider/Collider.h"
#include "../../Object/ObjectManager.h"
#include "../Info/ObjectInfo.h"
#include "../../Factory/EnemyAttackFactory.h"

#include "../../Common/Debug.h"

void EnemyAttackBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (enemyModel_.IsActive())
	{
		auto pos = MV1GetFramePosition(*enemyModel_->GetHandle(), frameIdx_);
		transform_->Pos() = Vector3{ pos.x, pos.y, pos.z };
		transform_->SetRotation({ Deg2Rad(45.0f),enemyTransform_->GetRotation().ToEuler().y , 0.0f});

	}
}

void EnemyAttackBehavior::Begin(ObjectManager& objectManager)
{
	objectManager.GetComponent<Collider>(ownerId_)->SetHitFunc(
		std::bind(
			&EnemyAttackBehavior::Hit, 
			this, 
			std::placeholders::_1, std::placeholders::_2
		)
	);

	enemyModel_ = objectManager.GetComponent<ModelRender>(enemyID_);
	//frameIdx_ = MV1SearchFrame(*enemyModel_->GetHandle(), L"‰Eèñæ");

	transform_ = objectManager.GetComponent<Transform>(ownerId_);

	enemyTransform_ = objectManager.GetComponent<Transform>(enemyID_);
}

void EnemyAttackBehavior::End(ObjectManager& objectManager)
{
}

void EnemyAttackBehavior::Hit(Collider& collider, ObjectManager& objectManager)
{
	auto atr = objectManager.GetComponent<ObjectInfo>(collider.GetOwnerID())->GetAttribute();
	DebugLog("“–‚½‚Á‚½", static_cast<int>(atr));
	if (atr == ObjectAttribute::Player)
	{
		DebugLog("“G‚É“–‚½‚Á‚½");
		objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
	}
}

void EnemyAttackBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(FactoryID::EnemyAttack).Destroy(ownerId_);
}
