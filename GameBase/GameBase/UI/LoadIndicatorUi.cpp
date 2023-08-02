#include <DxLib.h>
#include "LoadIndicatorUi.h"
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "../Common/Math.h"
#include "../Common/Debug.h"

LoadIndicatorUi::LoadIndicatorUi(const std::filesystem::path& indPath, const Vector2& indPos, const std::filesystem::path& fontPath) : UiBase(indPos)
{
	lpSceneMng.GetResourceMng().LoadTexture(handle_, indPath);
	lpSceneMng.GetResourceMng().LoadTexture(loadHandle_, fontPath);
	pos_ = indPos;
	// 描画角度初期化
	angle_ = Deg2Rad(0.0f);
	cnt_ = 0;
}

void LoadIndicatorUi::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	if (cnt_ % Interval == 0)
	{
		angle_ += Deg2Rad(360.0f / 12.0f);
	}
	cnt_++;
}

void LoadIndicatorUi::Draw(int mainScr)
{
	// インジケーター描画
	DrawRotaGraphF(pos_.x,pos_.y, 1.0, angle_, *handle_, true, false);
	// Now Loading描画
	DrawRotaGraphF(pos_.x, pos_.y + 70.0f, 1.0, 0.0f, *loadHandle_, true, false);
}
