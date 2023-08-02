#include "InputSetButton.h"
#include <DxLib.h>
#include "../Scene/OptionScene.h"
#include "../Common/ResourceMng.h"
#include "../Common/Input/InputType/Pad.h"

// キーボード用表示用indexテーブル
const std::unordered_map<int, int> keyImgTbl{
	{KEY_INPUT_0, 0},
	{KEY_INPUT_1, 1},
	{KEY_INPUT_2, 2},
	{KEY_INPUT_3, 3},
	{KEY_INPUT_4, 4},
	{KEY_INPUT_5, 5},
	{KEY_INPUT_6, 6},
	{KEY_INPUT_7, 7},
	{KEY_INPUT_8, 8},
	{KEY_INPUT_9, 9},
	{KEY_INPUT_B, 10},
	{KEY_INPUT_C, 11},
	{KEY_INPUT_E, 12},
	{KEY_INPUT_F, 13},
	{KEY_INPUT_G, 14},
	{KEY_INPUT_H, 15},
	{KEY_INPUT_I, 16},
	{KEY_INPUT_J, 17},
	{KEY_INPUT_K, 18},
	{KEY_INPUT_L, 19},
	{KEY_INPUT_M, 20},
	{KEY_INPUT_N, 21},
	{KEY_INPUT_O, 22},
	{KEY_INPUT_P, 23},
	{KEY_INPUT_Q, 24},
	{KEY_INPUT_R, 25},
	{KEY_INPUT_T, 26},
	{KEY_INPUT_U, 27},
	{KEY_INPUT_V, 28},
	{KEY_INPUT_X, 29},
	{KEY_INPUT_Y, 30},
	{KEY_INPUT_Z, 31},
	{KEY_INPUT_CAPSLOCK, 32},
	{KEY_INPUT_LALT, 33},
	{KEY_INPUT_LCONTROL, 34},
	{KEY_INPUT_LSHIFT, 35},
	{KEY_INPUT_RALT, 36},
	{KEY_INPUT_RCONTROL, 37},
	{KEY_INPUT_RSHIFT, 38},
	{KEY_INPUT_SPACE, 39},
	{KEY_INPUT_TAB, 40},
	{0, 41},
	{KEY_INPUT_UP, 42},
	{KEY_INPUT_DOWN, 43},
	{KEY_INPUT_LEFT, 44},
	{KEY_INPUT_RIGHT, 45},
	{-MOUSE_INPUT_LEFT, 46},
	{-MOUSE_INPUT_RIGHT,47}
};

// xboxコントローラー用表示用indexテーブル
const std::unordered_map<int, int> xboxImgTbl{
	{0, 0},
	{1, 1},
	{2, 2},
	{3, 3},
	{4, 4},
	{5, 5},
	{8, 6},
	{9, 7},
	{6, 8},
	{-1, 9},
};

InputSetButton::InputSetButton(InputID id, const Vector2& pos) :
	ButtonBase{pos}, id_{id} , draw_{&InputSetButton::DrawNonHitCursor}
{
	size_ = { 100.0f, 50.0f };
	lpSceneMng.GetResourceMng().LoadDivTexture(inputIDInfo_, "Resource/resource/InputInfo.png", { 4,1 }, { 163,47 });
	lpSceneMng.GetResourceMng().LoadTexture(button_, "Resource/resource/InputSetButton.png");
	lpSceneMng.GetResourceMng().LoadTexture(frame_, "Resource/resource/InputSetButtonFrame.png");
	
	if (lpSceneMng.GetController().GetCntType() == CntType::Key)
	{
		lpSceneMng.GetResourceMng().LoadDivTexture(inputInfo_, "Resource/resource/KeyInfo.png", { 4,12 }, { 219,47 });
		drawNowCode_ = &InputSetButton::DrawNowKeyCode;
	}
	else
	{
		auto& ctrl = static_cast<Pad&>(lpSceneMng.GetController());
		if ((ctrl.GetPadType() == DX_PADTYPE_DUAL_SHOCK_4) || (ctrl.GetPadType() == DX_PADTYPE_DUAL_SENSE))
		{
			// ps系のコントローラーの時
		}
		else
		{
			lpSceneMng.GetResourceMng().LoadDivTexture(inputInfo_, "Resource/resource/XboxInfo.png", { 5,2 }, { 169,47 });
			drawNowCode_ = &InputSetButton::DrawNowXboxCode;
		}
	}

}

void InputSetButton::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	nowCode_ = static_cast<OptionScene&>(scene).GetInputCode()[id_];
}

void InputSetButton::Draw(int mainScr)
{
	auto infoY = (size_.y - 47.0f) / 2.0f;
	DrawGraph(static_cast<int>(pos_.x), static_cast<int>(pos_.y), *button_, true);
	DrawGraph(static_cast<int>(pos_.x + 80.0f), static_cast<int>(pos_.y + infoY), inputIDInfo_[static_cast<size_t>(id_)], true);

	(this->*drawNowCode_)();
	(this->*draw_)();
}

void InputSetButton::Push(BaseScene& scene, Controller& controller)
{
	static_cast<OptionScene&>(scene).SetInputID(id_);
	scene.ChangeSceneID(SceneID::InputSet);
}

void InputSetButton::HitCursor(void)
{
	isHitCursor_ = true;
	draw_ = &InputSetButton::DrawHitCursor;
}

void InputSetButton::NotHitCursor(void)
{
	isHitCursor_ = false;
	draw_ = &InputSetButton::DrawNonHitCursor;
}

void InputSetButton::Loaded(BaseScene& scene)
{
	GetGraphSizeF(*button_, &size_.x, &size_.y);
	pos_ -= size_ / 2.0f;
	nowCode_ = static_cast<OptionScene&>(scene).GetInputCode()[id_];

}

void InputSetButton::DrawHitCursor(void)
{
	DrawGraph(static_cast<int>(pos_.x), static_cast<int>(pos_.y),  *frame_, true);
}

void InputSetButton::DrawNonHitCursor(void)
{
}

void InputSetButton::DrawNowXboxCode(void)
{
	auto infoY = (size_.y - 47.0f) / 2.0f;
	auto tmpIdx = 0;
	if (xboxImgTbl.count(nowCode_))
	{
		tmpIdx = xboxImgTbl.at(nowCode_);
	}
	DrawGraph(static_cast<int>(pos_.x + (479.0f - 80.0f - 169.0f)), static_cast<int>(pos_.y + infoY), inputInfo_[tmpIdx], true);
}

void InputSetButton::DrawNowKeyCode(void)
{
	auto infoY = (size_.y - 47.0f) / 2.0f;
	auto tmpIdx = 0;
	if (keyImgTbl.count(nowCode_))
	{
		tmpIdx = keyImgTbl.at(nowCode_);
	}
	DrawGraph(static_cast<int>(pos_.x + (479.0f - 219.0f)), static_cast<int>(pos_.y + infoY), inputInfo_[tmpIdx], true);
}

void InputSetButton::DrawNowPsCode(void)
{
}
