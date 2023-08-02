#include "SwitchButton.h"
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"

SwitchButton::SwitchButton(
	const PushFunc& pushFunc,
	const UpdateFunc& updateFunc,
	const std::filesystem::path& onPath,
	const std::filesystem::path& offPath, 
	const std::filesystem::path& cursorPath, 
	const Vector2& pos) :
	ButtonBase{pos}, pushFunc_{pushFunc}, updateFunc_{updateFunc}
{
	lpSceneMng.GetResourceMng().LoadTexture(onBtnHandle_, onPath);
	lpSceneMng.GetResourceMng().LoadTexture(offBtnHandle_, offPath);
	lpSceneMng.GetResourceMng().LoadTexture(hitCursorHandle_, cursorPath);
	drawButton_ = &SwitchButton::DrawOnButton;
	drawFrame_ = &SwitchButton::DrawNotCursor;
	isOn_ = false;
}

void SwitchButton::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	isOn_ = updateFunc_(scene);
	if (isOn_)
	{
		drawButton_ = &SwitchButton::DrawOnButton;
	}
	else
	{
		drawButton_ = &SwitchButton::DrawOffButton;
	}
}

void SwitchButton::Draw(int mainScr)
{
	(this->*drawButton_)();
	(this->*drawFrame_)();
}

void SwitchButton::Push(BaseScene& scene, Controller& controller)
{
	isOn_ = !isOn_;
	pushFunc_(scene, isOn_);
}

void SwitchButton::HitCursor(void)
{
	drawFrame_ = &SwitchButton::DrawOnCursor;
	isHitCursor_ = true;
}

void SwitchButton::NotHitCursor(void)
{
	drawFrame_ = &SwitchButton::DrawNotCursor;
	isHitCursor_ = false;
}

void SwitchButton::Loaded(BaseScene& scene)
{
	GetGraphSizeF(*onBtnHandle_, &size_.x, &size_.y);
	pos_ -= size_ / 2.0f;
	isOn_ = updateFunc_(scene);
	if (isOn_)
	{
		drawButton_ = &SwitchButton::DrawOnButton;
	}
	else
	{
		drawButton_ = &SwitchButton::DrawOffButton;
	}
}

void SwitchButton::DrawOnButton(void)
{
	DrawGraphF(pos_.x, pos_.y, *onBtnHandle_, true);
}

void SwitchButton::DrawOffButton(void)
{
	DrawGraphF(pos_.x, pos_.y, *offBtnHandle_, true);
}

void SwitchButton::DrawOnCursor(void)
{
	DrawGraphF(pos_.x, pos_.y, *hitCursorHandle_, true);
}

void SwitchButton::DrawNotCursor(void)
{
}
