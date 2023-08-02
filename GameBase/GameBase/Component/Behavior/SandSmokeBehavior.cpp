#include "SandSmokeBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../../Factory/SandSmokeEffect.h"

#include "../../Common/Debug.h"

SandSmokeBehavior::SandSmokeBehavior()
{
	interval_ = 0.0f;
	isInterval_ = false;
	isPlay_ = false;
}

void SandSmokeBehavior::Play(void)
{
	interval_ = 0.0f;
	isInterval_ = true;
	isPlay_ = false;
}

void SandSmokeBehavior::Play(float time)
{
	interval_ = time;
	isInterval_ = true;
	isPlay_ = false;
}

void SandSmokeBehavior::Stop(void)
{
	effectRender_->Stop();
}

void SandSmokeBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	UpdateSandSmokeBehavior(delta);
	auto player = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	if (player.IsActive())
	{
		transform_->Pos() = player->GetPos();
		auto playerRot = player->GetRotation().ToEuler();
		transform_->SetRotation({ playerRot.x, playerRot.y, playerRot.z });
	}
}

void SandSmokeBehavior::Begin(ObjectManager& objectManager)
{
	effectRender_ = objectManager.GetComponent<EffectRender>(ownerId_);
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
}

void SandSmokeBehavior::UpdateSandSmokeBehavior(float delta)
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