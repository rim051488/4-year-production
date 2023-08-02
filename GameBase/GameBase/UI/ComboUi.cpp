#include "ComboUi.h"
#include <DxLib.h>
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "../Common/Debug.h"
#include "../Object/ObjectManager.h"
#include "../Component/Behavior/PlayerBehavior.h"
#include "../Component/Info/ObjectInfo.h"

ComboUi::ComboUi(const std::filesystem::path& numPath, const Vector2I& div, const Vector2I& size, const std::filesystem::path& fontPath, const Vector2& pos, const Vector2& interval) : UiBase(pos)
{
	size_ = size;
	lpSceneMng.GetResourceMng().LoadDivTexture(divHandle_, numPath, div, size_);
	lpSceneMng.GetResourceMng().LoadTexture(handle_, fontPath);
	pos_ = pos;
	interval_ = interval;
	combo_ = 0;
}

ComboUi::~ComboUi()
{
}

void ComboUi::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	auto player = objMng.GetComponent<PlayerBehavior>(objMng.GetPlayerID());
	combo_ = player->GetComboNum();
}

void ComboUi::Draw(int mainScr)
{
	if (!combo_)
	{
		return;
	}

	if (combo_ / 100 != 0)
	{
		// 3Œ…–Ú
		DrawGraph(static_cast<int>(pos_.x - size_.x), static_cast<int>(pos_.y), divHandle_[combo_ / 100], true);
	}
	if (combo_ / 10 != 0)
	{
		// 2Œ…–Ú
		DrawGraph(static_cast<int>(pos_.x - size_.x / 2), static_cast<int>(pos_.y), divHandle_[combo_ / 10], true);
	}
	// 1Œ…–Ú
	DrawGraph(static_cast<int>(pos_.x), static_cast<int>(pos_.y), divHandle_[combo_ % 10], true);
	DrawGraph(static_cast<int>(pos_.x + interval_.x), static_cast<int>(pos_.y + interval_.y), *handle_, true);
}
