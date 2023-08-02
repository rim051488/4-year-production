#include "PlayerBulletBehavior.h"
#include "../../Factory/EnemyBullet.h"
#include "../Info/ObjectInfo.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/Debug.h"
#include "PlayerBehavior.h"
#include "../../Common/SoundPross.h"
constexpr float speed_{ 3800.0f };


void PlayerBulletBehavior::Begin(ObjectManager& objectManager)
{
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
	transform_->Scale() = Vector3{ 70.0f,70.0f,70.0f };
	collider_ = objectManager.GetComponent<SphereCollider>(ownerId_);
	collider_->SetHitFunc(std::bind(&PlayerBulletBehavior::OnHit, this, std::placeholders::_1, std::placeholders::_2));
	startPos_ = transform_->GetPos();
	lpSooundPross.PlayBackSound(SOUNDNAME_SE::enemyAttack, false);
}

void PlayerBulletBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	// 前方方向に移動
	transform_->Pos() -= transform_->GetForward() * (speed_ * delta);
	auto info = objectManager.GetComponent<ObjectInfo>(ownerId_);

	if ((transform_->GetPos() - startPos_).SqMagnitude() >= Square(5000.0f))
	{
		// 一定の距離になったら自分自身を破棄する
		objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
	}
}

void PlayerBulletBehavior::OnHit(Collider& col, ObjectManager& objectManager)
{
	auto info = objectManager.GetComponent<ObjectInfo>(col.GetOwnerID());
	// ヒット時に処理を書く
	objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
}

void PlayerBulletBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(FactoryID::PlayerBullet).Destroy(ownerId_);
}
