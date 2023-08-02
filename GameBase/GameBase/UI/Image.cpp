#include "Image.h"
#include <DxLib.h>
#include "../Common/ResourceMng.h"
#include "../SceneManager.h"

#include "../Common/Debug.h"

// ’P‘Ì‰æ‘œ
Image::Image(const std::filesystem::path& path, const Vector2 pos, ViewType type) :
	UiBase{pos}, time_{0.0f}
{
	switch (type)
	{
	case ViewType::Non:
		draw_ = &Image::DrawNon;
		break;
	case ViewType::Blink:
		draw_ = &Image::DrawBlink;
		break;
	default:
		break;
	}
	lpSceneMng.GetResourceMng().LoadTexture(handle_, path);
}

// •ªŠ„‰æ‘œ
Image::Image(const std::filesystem::path& path, const Vector2I& div, const Vector2I& size, const Vector2& pos, const Vector2& interval) :
	UiBase{ pos }, draw_{&Image::DrawNon}
{
	div_ = div;
	interval_ = interval;
	lpSceneMng.GetResourceMng().LoadDivTexture(divHandle_, path, div, size);
}

void Image::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	time_ += delta;
}

void Image::Draw(int mainScr)
{
	(this->*draw_)();
}

void Image::DrawNon(void)
{
	if (*handle_ != -1)
	{
		DrawRotaGraphF(pos_.x, pos_.y, 1.0f, 0.0f, *handle_, true, false);
	}
	if (*divHandle_ != -1)
	{
		auto divCnt = div_.x * div_.y;
		for (int i = 0; i < divCnt; i++)
		{
			DrawRotaGraphF(pos_.x, pos_.y + interval_.y * i, 1.0f, 0.0f, divHandle_[i], true, false);
		}
	}
}

void Image::DrawBlink(void)
{
	if (time_ >= 0.5f)
	{
		DrawNon();
		if (time_ >= 1.0f)
		{
			time_ = 0.0f;
		}
	}
}
