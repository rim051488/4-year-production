#include <DxLib.h>
#include "EffekseerForDXLib.h"
#include "../../Common/ResourceMng.h"
#include "../../SceneManager.h"
#include "../../Object/ObjectManager.h"
#include "EffectRender.h"
#include "../../Common/Debug.h"

//
EffectRender::EffectRender() :
	playingHandle_{-1}
{
}

EffectRender::~EffectRender()
{

}

bool EffectRender::IsEnd(void)
{
	return IsEffekseer3DEffectPlaying(playingHandle_) == -1;
}

void EffectRender::Play(void)
{
	if (playingHandle_ != -1)
	{
		StopEffekseer3DEffect(playingHandle_);
	}

	playingHandle_ = PlayEffekseer3DEffect(*handle_);
}

void EffectRender::Stop(void)
{
	StopEffekseer3DEffect(playingHandle_);
	playingHandle_ = -1;
}

bool EffectRender::Init(void)
{
	/*int num_;
	EffectInfo *info_;
	EffectInit *effectInit;*/

	//effectInit = baseInit_[EffectMax];

	return false;
}

void EffectRender::Load(const std::filesystem::path& path)
{
	SetUseASyncLoadFlag(false);
	lpSceneMng.GetResourceMng().LoadEffect(handle_, path);
	SetUseASyncLoadFlag(true);
	//LoadEffect();
}

void EffectRender::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (transform_.IsActive() && playingHandle_ != -1)
	{
		SetPosPlayingEffekseer3DEffect(playingHandle_, transform_->GetPos().x, transform_->GetPos().y, transform_->GetPos().z);
		auto rot = transform_->GetRotation().ToEuler();
		SetRotationPlayingEffekseer3DEffect(playingHandle_, rot.x, rot.y, rot.z);
		auto scale = transform_->GetScale();
		SetScalePlayingEffekseer3DEffect(playingHandle_, scale.x, scale.y, scale.z);
		
		// デルタタイムをもとに再生速度を決める
		SetSpeedPlayingEffekseer3DEffect(playingHandle_, delta / 0.016f);
	}
	
}


void EffectRender::UpdateEffekseer(void)
{

	playPos_ = transform_.Get()->GetPos();
	if (effectTime_ <= effectTimeMax_.desEx) {
		playingHandle_ = PlayEffekseer3DEffect(effectResourceHandle);
	}
	else
	{
		isEnd_ = true;
	}
	SetPosPlayingEffekseer3DEffect(playingHandle_, playPos_.x, playPos_.y, playPos_.z);

	
	UpdateEffekseer3D();
}
//void EffectRender::LoadEffect(void)
//{
//	effectResourceHandle = LoadEffekseerEffect("Resource/Effect/FireEx.efkefc", 1.0f);
//
//}

void EffectRender::End(ObjectManager& objectManager)
{
	Render::End(objectManager);
	Stop();
}

	










