#include "BeemEffectBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../../Factory/BeemEffect.h"

BeemEffectBehavior::BeemEffectBehavior()
{
}

void BeemEffectBehavior::Start(void)
{
	effectRender_->Play();
}

void BeemEffectBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (playerT_.IsActive())
	{
		// プレイヤーがアクティブの時座標とy軸角度を同期する
		transform_->SetRotation({ 0.0f, playerT_->GetRotation().ToEuler().y, 0.0f });
		transform_->Pos() = playerT_->GetPos() + (transform_->GetUp() * 50.0f);
	}
}

void BeemEffectBehavior::Begin(ObjectManager& objectManager)
{
	effectRender_ = objectManager.GetComponent<EffectRender>(ownerId_);
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
	playerT_ = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
}

void BeemEffectBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(FactoryID::BeemEffect).Destroy(ownerId_);
}
