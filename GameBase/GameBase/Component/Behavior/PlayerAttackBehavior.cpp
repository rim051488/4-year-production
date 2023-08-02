#include "PlayerAttackBehavior.h"
#include <DxLib.h>
#include "../Collider/Collider.h"
#include "../../Object/ObjectManager.h"
#include "../Info/ObjectInfo.h"
#include "../../Factory/PlayerAttackFactory.h"

#include "../../Common/Debug.h"

void PlayerAttackBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	//delta_ = delta;
	//// 攻撃のボーンの座標に合わせる
	//if (playerModel_.IsActive())
	//{
	//	auto pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx_);
	//	transform_->Pos() = Vector3{ pos.x, pos.y, pos.z };
	//}
	//auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	//if (player.IsActive())
	//{
	//	transform_->SetRotation({ Deg2Rad(90.0f),0.0f,-player->GetRotation().ToEuler().y});
	//}
	//(this->*update_)(objectManager, delta);
	//if (isAttackInterval_)
	//{
	//	hitTime_ += delta_;
	//	if (hitTime_ >= 2.0f)
	//	{
	//		// ↓攻撃できなくなるバグの原因おそらくこれ
	//		//auto playerBehavior = objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID());
	//		//playerBehavior->ResetCombo(objectManager);
	//		isAttackInterval_ = false;
	//	}
	//}

	if (playerModel_.IsActive())
	{
		auto pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx_);
		transform_->Pos() = Vector3{ pos.x, pos.y, pos.z };
		transform_->SetRotation({ Deg2Rad(45.0f),playerTransform_->GetRotation().ToEuler().y , 0.0f });
	}
}

void PlayerAttackBehavior::Effect(ObjectManager& objectManager)
{
	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	if (!player.IsActive())
	{
		return;
	}

	// エフェクト関係
	for (auto& e : effect_)
	{
		auto& flag = std::get<2>(e);
		if (flag)
		{
			auto& factory = std::get<0>(e);
			auto& pos = std::get<1>(e);
			auto id = objectManager.CreateFromFactory(factory, objectManager.GetPlayerID(), player->GetPos() + (player->GetRotation() * pos), {0.0f,player->GetRotation().ToEuler().y ,0.0f});
			objectManager.Begin(id);
			flag = false;
		}
	}
}

void PlayerAttackBehavior::AddEffect(FactoryID id, ObjectManager& objectManager, const Vector3& offset)
{
	// エフェクトの種類取得
	auto& effect = effect_.at(static_cast<int>(id));
	// 座標格納
	auto& pos = std::get<1>(effect);		// 座標格納場所
	//auto frameIdx = MV1SearchFrame(*playerModel_->GetHandle(), frameNameCenter_.c_str());			// 基準にするフレーム
	//pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx);								// 座標格納

	pos = offset;

	// 発動フラグ
	auto& flag = std::get<2>(effect);
	flag = true;
}

void PlayerAttackBehavior::Begin(ObjectManager& objectManager)
{
	//auto player = (objectManager.GetComponent<Transform>(objectManager.GetPlayerID()));
	//playerModel_ = objectManager.GetComponent<ModelRender>(objectManager.GetPlayerID());
	//transform_ = objectManager.GetComponent<Transform>(ownerId_);
	//frameNameBlade_ = L"Blade-R";
	//frameNameCenter_ = L"Center";
	//frameIdx_ = MV1SearchFrame(*playerModel_->GetHandle(), frameNameBlade_.c_str());
	//auto pos = MV1GetFramePosition(*playerModel_->GetHandle(), frameIdx_);
	//transform_->Pos() = player->GetPos();
	//collider_ = objectManager.GetComponent<Collider>(ownerId_);
	//collider_->SetHitFunc(std::bind(&PlayerAttackBehavior::OnHit, this, std::placeholders::_1, std::placeholders::_2));
	//collider_->SetActiveFlag(false);
	//attackTime_ = 2.0f;
	//// コンボごとの攻撃アニメーション
	//combo_ = objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID())->GetAtkCnt() + 1;
	//if (combo_)
	//{
	//	if (combo_ % 3 == 1)
	//	{
	//		AddEffect(FactoryID::VerticalEffect, objectManager, {0.0f,0.0f, 0.0f});
	//	
	//	}
	//	else if (combo_ % 3 == 2)
	//	{
	//		AddEffect(FactoryID::HorizontalEffect, objectManager, { 0.0f,0.0f, -100000000.0f });
	//	
	//	}
	//	else if (combo_ % 3 == 0)
	//	{
	//		AddEffect(FactoryID::ThrustEffect, objectManager, { 0.0f,35.0f, -100.0f });
	//	
	//	}
	//}
	//update_ = &PlayerAttackBehavior::UpdateNonAttack;

	objectManager.GetComponent<Collider>(ownerId_)->SetHitFunc(
		std::bind(
			&PlayerAttackBehavior::OnHit,
			this,
			std::placeholders::_1, std::placeholders::_2
		)
	);

	playerModel_ = objectManager.GetComponent<ModelRender>(playerID_);
	frameIdx_ = MV1SearchFrame(*playerModel_->GetHandle(), L"右手首先");

	transform_ = objectManager.GetComponent<Transform>(ownerId_);

	playerTransform_ = objectManager.GetComponent<Transform>(playerID_);

}

void PlayerAttackBehavior::End(ObjectManager& objectManager)
{
	attackStartTime_ = 0.0f;
}

void PlayerAttackBehavior::OnHit(Collider& col, ObjectManager& objectManager)
{
	//auto atr = objectManager.GetComponent<ObjectInfo>(col.GetOwnerID())->GetAttribute();
	//if (atr == ObjectAttribute::Enemy)
	//{
	//	hitTime_ = 0.0f;
	//	isAttackInterval_ = true;
	//}
	auto atr = objectManager.GetComponent<ObjectInfo>(col.GetOwnerID())->GetAttribute();
	DebugLog("当たった", static_cast<int>(atr));
	if (atr == ObjectAttribute::Player)
	{
		DebugLog("敵に当たった");
		objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
	}

}

void PlayerAttackBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(FactoryID::PlayerAttack).Destroy(ownerId_);
}
