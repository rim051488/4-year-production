#include "FadeLoading.h"
#include <DxLib.h>
#include "../../UI/UiManager.h"

#include "../..//Common/Debug.h"

FadeLoading::FadeLoading(SceneUptr befor, SceneUptr after, float transTime) :
	Loading{std::move(befor), std::move(after),transTime},
	update_{ &FadeLoading::UpdateFadeOut },
	draw_{ &FadeLoading::DrawFadeOut }
{
}

FadeLoading::FadeLoading(SceneUptr after, float transTime) :
	Loading{nullptr, std::move(after),transTime },
	update_{ &FadeLoading::UpdateLoading },
	draw_{ &FadeLoading::DrawLoading }
{
}

void FadeLoading::Update(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);
	(this->*update_)(delta, controller);
}

void FadeLoading::DrawScene(void)
{
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	(this->*draw_)();
}

void FadeLoading::UpdateFadeOut(float delta, Controller& controller)
{
	if ((stepTime_ += delta) >= transTime_)
	{
		stepTime_ = 0.0f;
		update_ = &FadeLoading::UpdateLoading;
		draw_ = &FadeLoading::DrawLoading;
	}
}

void FadeLoading::UpdateLoading(float delta, Controller& controller)
{
	if (after_->IsLoaded())
	{
		stepTime_ = 0.0f;
		after_->Loaded(controller);
		after_->DrawScene();
		update_ = &FadeLoading::UpdateFadesIn;
		draw_ = &FadeLoading::DrawFadeIn;
	}
}

void FadeLoading::UpdateFadesIn(float delta, Controller& controller)
{
	if ((stepTime_ += delta) >= transTime_)
	{
		ChangeSceneID(after_->GetID());
	}
}

void FadeLoading::DrawFadeIn(void)
{
	uiMng_->Draw(*screenHandle_);
	float alpha = stepTime_ / transTime_;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * alpha));
	after_->Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void FadeLoading::DrawLoading(void)
{
	uiMng_->Draw(*screenHandle_);
}

void FadeLoading::DrawFadeOut(void)
{
	uiMng_->Draw(*screenHandle_);

	float alpha = stepTime_ / transTime_;
	alpha = 1.0f - alpha;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * alpha));
	befor_->Draw();
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}
