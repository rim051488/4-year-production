#include "SandSmokeSlowMoveBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../../Factory/SandSmokeSlowMoveEffect.h"

#include "../../Common/Debug.h"

SandSmokeSlowMoveBehavior::SandSmokeSlowMoveBehavior()
{
	interval_ = 0.0f;
	isInterval_ = false;
	isPlay_ = false;
}

void SandSmokeSlowMoveBehavior::Play(void)
{
	interval_ = 0.0f;
	isInterval_ = true;
	isPlay_ = false;
}

void SandSmokeSlowMoveBehavior::Play(float time)
{
	interval_ = time;
	isInterval_ = true;
	isPlay_ = false;
}

void SandSmokeSlowMoveBehavior::Stop(void)
{
	effectRender_->Stop();
}

void SandSmokeSlowMoveBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	UpdateSandSmokeDiffusionBehavior(delta);
	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	if (player.IsActive())
	{
		transform_->Pos() = player->GetPos();
		auto playerRot = player->GetRotation().ToEuler();
		transform_->SetRotation({ playerRot.x, playerRot.y, playerRot.z });
	}
}

void SandSmokeSlowMoveBehavior::Begin(ObjectManager& objectManager)
{
	effectRender_ = objectManager.GetComponent<EffectRender>(ownerId_);
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
}

void SandSmokeSlowMoveBehavior::UpdateSandSmokeDiffusionBehavior(float delta)
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
