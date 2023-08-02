#include "PlayerSkillBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../Info/ObjectInfo.h"
#include "../../Factory/PlayerSkillFactory.h"

#include "PlayerBehavior.h"
#include "EnemyBehavior.h"
#include "../../UI/UiID.h"
#include "BeemEffectBehavior.h"

#include "../../Common/Debug.h"

PlayerSkillBehavior::PlayerSkillBehavior() :
	attackTime_{ 0.0f }, attackStartTime_{ 0.0f }, animTime_{ 0.0f }, update_{ nullptr }
{
}

void PlayerSkillBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());

	if (player.IsActive())
	{
		// プレイヤーと座標を同期させる
		transform_->Pos() = player->GetPos();
		(this->*update_)(objectManager, delta);

		// プレイヤーと回転を同期させる
		auto plRot = player->GetRotation().ToEuler();
		transform_->SetRotation({ Deg2Rad(90.0f),0.0f,-plRot.y });
	}
}

void PlayerSkillBehavior::Begin(ObjectManager& objectManager)
{
	auto player = (objectManager.GetComponent<Transform>(objectManager.GetPlayerID()));
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
	transform_->Pos() = player->GetPos();

	collider_ = objectManager.GetComponent<Collider>(ownerId_);
	collider_->SetHitFunc(std::bind(&PlayerSkillBehavior::OnHit, this, std::placeholders::_1, std::placeholders::_2));
	collider_->SetActiveFlag(false);
	attackTime_ = 2.6f;

	update_ = &PlayerSkillBehavior::UpdateNonAttack;

	effectID_ = objectManager.CreateFromFactory(FactoryID::BeemEffect, ownerId_, transform_->GetPos());
	objectManager.Begin(effectID_);
}

void PlayerSkillBehavior::End(ObjectManager& objectManager)
{
	attackStartTime_ = 0.0f;
	auto effect = objectManager.GetComponent<ObjectInfo>(effectID_);
	effect->Destory();
}

void PlayerSkillBehavior::OnHit(Collider& col, ObjectManager& objectManager)
{
}

void PlayerSkillBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(FactoryID::PlayerSkill).Destroy(ownerId_);
}

void PlayerSkillBehavior::UpdateAttack(ObjectManager& objectManager, float delta)
{
	attackTime_ -= delta;
	// エフェクト更新
	if (attackTime_ <= 0.0f)
	{
		objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID())->EndSkill();
		objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
	}
}

void PlayerSkillBehavior::UpdateNonAttack(ObjectManager& objectManager, float delta)
{
	attackStartTime_ -= delta;
	if (attackStartTime_ <= 0.0f)
	{
		update_ = &PlayerSkillBehavior::UpdateAttack;
		collider_->SetActiveFlag(true);
		// エフェクト発動
		auto beem = objectManager.GetComponent<BeemEffectBehavior>(effectID_);
		if (beem.IsActive())
		{
			beem->Start();
		}
	}
}
