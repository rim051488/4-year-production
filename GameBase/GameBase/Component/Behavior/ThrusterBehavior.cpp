#include "ThrusterBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../../Factory/ThrustEffect.h"

#include "../../Common/Debug.h"

ThrusterBehavior::ThrusterBehavior()
{
	interval_ = 0.0f;
	isInterval_ = false;
	isPlay_ = false;
}

void ThrusterBehavior::Play(void)
{
	interval_ = 0.0f;
	isInterval_ = true;
	isPlay_ = false;
}

void ThrusterBehavior::Play(float time)
{
	interval_ = time;
	isInterval_ = true;
	isPlay_ = false;
}

void ThrusterBehavior::Stop(void)
{
	effectRender_->Stop();
}

void ThrusterBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	UpdateThrusterInterval(delta);
	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	if (player.IsActive())
	{
		transform_->Pos() = player->GetPos();
		auto playerRot = player->GetRotation().ToEuler();
		transform_->SetRotation({ playerRot.x, playerRot.y, playerRot.z });
	}
}

void ThrusterBehavior::Begin(ObjectManager& objectManager)
{
	effectRender_ = objectManager.GetComponent<EffectRender>(ownerId_);
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
}

void ThrusterBehavior::UpdateThrusterInterval(float delta)
{
	if (!isInterval_)
	{
		return;
	}

	interval_ -= delta;
	if (interval_ <= 0.0f)
	{
		effectRender_->Play();
		isInterval_ = false;
		isPlay_ = true;
	}
}
