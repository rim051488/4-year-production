#include "Button.h"
#include <DxLib.h>
#include "../Common/ResourceMng.h"
#include "../SceneManager.h"
#include "../Common/Debug.h"

Button::Button(const PushFunc& pushFunc, const std::filesystem::path& btnPath, const std::filesystem::path& hitCursorPath, const Vector2& pos) :
	ButtonBase{pos}, pushFunc_{pushFunc}
{
	draw_ = &Button::DrawNotHitCursor;
	lpSceneMng.GetResourceMng().LoadTexture(btnHandle_, btnPath);
	lpSceneMng.GetResourceMng().LoadTexture(hitCursorHandle_, hitCursorPath);
	pos_ = pos;
}

void Button::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
}

void Button::Draw(int mainScr)
{
	(this->*draw_)();
}

void Button::Push(BaseScene& scene, Controller& controller)
{
	pushFunc_(scene);
}

void Button::HitCursor(void)
{
	draw_ = &Button::DrawHitCursor;
	isHitCursor_ = true;
}

void Button::NotHitCursor(void)
{
	draw_ = &Button::DrawNotHitCursor;
	isHitCursor_ = false;
}

void Button::Loaded(BaseScene& scene)
{
	GetGraphSizeF(*btnHandle_, &size_.x, &size_.y);
	pos_ -= size_ / 2.0f;
}

void Button::DrawHitCursor(void)
{
	DrawGraphF(pos_.x, pos_.y, *btnHandle_, true);
	DrawGraphF(pos_.x, pos_.y, *hitCursorHandle_, true);
}


void Button::DrawNotHitCursor(void)
{
	DrawGraphF(pos_.x, pos_.y, *btnHandle_, true);
}
