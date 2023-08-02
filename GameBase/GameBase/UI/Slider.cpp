#include "Slider.h"
#include <DxLib.h>
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "../Common/Input/Controller.h"

#include "../Common/Debug.h"

Slider::Slider(
	const ValueGetFunc& valueGetFunc,
	const std::filesystem::path& sliderBarPath, 
	const std::filesystem::path& sliderTabPath, 
	const std::filesystem::path& sliderValuePath, 
	float defaultValue, 
	const Vector2& pos) :
	ButtonBase{pos}, valueGetFunc_{valueGetFunc}, value_{ defaultValue }
{
	lpSceneMng.GetResourceMng().LoadTexture(sliderBar_, sliderBarPath);
	lpSceneMng.GetResourceMng().LoadTexture(sliderTab_, sliderTabPath);
	lpSceneMng.GetResourceMng().LoadTexture(sliderValue_, sliderValuePath);
}

void Slider::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
}

void Slider::Draw(int mainScr)
{
	DrawGraph(static_cast<int>(pos_.x), static_cast<int>(pos_.y), *sliderBar_, true);

	auto norm = (value_ + (sliderTabSize_.y / 2.0f)) / size_.x;
	DrawRectGraph(
		static_cast<int>(pos_.x), static_cast<int>(pos_.y),
		0, 0,
		static_cast<int>(sliderValueSize_.x * norm - sliderTabSize_.x), static_cast<int>(sliderValueSize_.y),
		*sliderValue_, true
	);

	DrawGraph(static_cast<int>(sliderTabPos_.x), static_cast<int>(sliderTabPos_.y), *sliderTab_, true);
	
}

void Slider::Push(BaseScene& scene, Controller& controller)
{
	value_ = Clamp((controller.GetCursorPos().x), pos_.x, (pos_.x + size_.x) - sliderTabSize_.x);
	sliderTabPos_.x = value_;
	value_ -= pos_.x;
	valueGetFunc_(value_/size_.x);
}

void Slider::HitCursor(void)
{
}

void Slider::NotHitCursor(void)
{
}

bool Slider::IsDecision(Controller& controller) const
{
	return controller.PressDecision();
}

void Slider::Loaded(BaseScene& scene)
{
	GetGraphSizeF(*sliderTab_, &sliderTabSize_.x, &sliderTabSize_.y);
	GetGraphSizeF(*sliderBar_, &size_.x, &size_.y);
	sliderTabPos_.x = pos_.x;
	sliderTabPos_.y = (pos_.y + size_.y / 2.0f) - sliderTabSize_.y / 2.0f;
	size_.y = sliderTabSize_.y;

	value_ = value_ * (size_.x - sliderTabSize_.x);
	value_ = Clamp(value_ + pos_.x, pos_.x, (pos_.x + size_.x) - sliderTabSize_.x);
	sliderTabPos_.x = value_;
	value_ -= pos_.x;
	GetGraphSizeF(*sliderValue_, &sliderValueSize_.x, &sliderValueSize_.y);
}
