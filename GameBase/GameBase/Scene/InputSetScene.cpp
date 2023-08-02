#include "InputSetScene.h"
#include <DxLib.h>
#include <span>
#include "OptionScene.h"
#include "../Common/Input/InputType/Keyboard.h"
#include "../Common/Input/InputType/Pad.h"
#include "../Common/ResourceMng.h"

// セットしないキー
const std::vector<int> keyIgnoreTbl{
	KEY_INPUT_W,
	KEY_INPUT_A,
	KEY_INPUT_S,
	KEY_INPUT_D,
	KEY_INPUT_F1,
	KEY_INPUT_F2,
	KEY_INPUT_F3,
	KEY_INPUT_F4,
	KEY_INPUT_F5,
	KEY_INPUT_F6,
	KEY_INPUT_F7,
	KEY_INPUT_F8,
	KEY_INPUT_F9,
	KEY_INPUT_F10,
	KEY_INPUT_MINUS,
	KEY_INPUT_YEN,
	KEY_INPUT_PREVTRACK,
	KEY_INPUT_PERIOD,
	KEY_INPUT_SLASH,
	KEY_INPUT_LALT,
	KEY_INPUT_RALT,
	KEY_INPUT_SCROLL,
	KEY_INPUT_SEMICOLON,
	KEY_INPUT_COLON,
	KEY_INPUT_LBRACKET,
	KEY_INPUT_RBRACKET,
	KEY_INPUT_AT,
	KEY_INPUT_BACKSLASH,
	KEY_INPUT_COMMA,
	KEY_INPUT_KANJI,
	KEY_INPUT_CONVERT,
	KEY_INPUT_NOCONVERT,
	KEY_INPUT_KANA,
	KEY_INPUT_APPS,
	KEY_INPUT_SYSRQ,
	KEY_INPUT_PAUSE,
	KEY_INPUT_LWIN,
	KEY_INPUT_RWIN,
	KEY_INPUT_PGUP,
	KEY_INPUT_PGDN,
	KEY_INPUT_END,
	KEY_INPUT_HOME,
	KEY_INPUT_INSERT,
	KEY_INPUT_DELETE,
	KEY_INPUT_ESCAPE
};

std::vector<int> xboxPadIgnoreTbl{
	6,
	7
};

std::vector<int> psPadIgnoreTbl{
	8,
	9
};


InputSetScene::InputSetScene(SceneUptr optionScene, InputID id) :
	BaseScene{ScreenID::InputSet,SceneID::InputSet}, optionScene_{std::move(optionScene)}, id_{id},
	step_{0.0f}
{
	SetMakeSceneFunc([this](auto own) { 
		optionScene_->ChangeSceneID(SceneID::Option);
		return std::move(optionScene_); 
		}, SceneID::Option);
	update_ = &InputSetScene::UpdateWait;
	draw_ = &InputSetScene::DrawSet;

	SetUseASyncLoadFlag(false);
	lpSceneMng.GetResourceMng().LoadTexture(mess_, "Resource/resource/InputSetMess.png");
	lpSceneMng.GetResourceMng().LoadTexture(mess2_, "Resource/resource/InputSetMess2.png");
	lpSceneMng.GetResourceMng().LoadTexture(mess3_, "Resource/resource/InputSetMess3.png");

	isSelectSuccess_ = true;

	SetUseASyncLoadFlag(true);
}

void InputSetScene::Update(float delta, Controller& controller)
{
	(this->*update_)(delta, controller);
}

void InputSetScene::DrawScene(void)
{
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	optionScene_->Draw();
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 128);
	DrawBox(0, 0, SceneManager::screenSize_<int>.x, SceneManager::screenSize_<int>.y, 0x000000, true);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 128);
	(this->*draw_)();
	
}

void InputSetScene::UpdateWait(float delta, Controller& controller)
{
	step_ += delta;

	if (step_ >= 0.5f)
	{
		step_ = 0.0f;
		if (controller.GetCntType() == CntType::Key)
		{
			// キーボードの時
			update_ = &InputSetScene::UpdateKeyboardSet;
		}
		else
		{
			// ゲームパッドの時
			update_ = &InputSetScene::UpdatePadSet;
		}
		
	}
}

void InputSetScene::UpdateKeyboardSet(float delta, Controller& controller)
{
	auto& data = static_cast<Keyboard&>(controller).GetKeyData();

	for (size_t i = 0; i < data.size(); i++)
	{
		

		if (data[i])
		{
			if (std::find(keyIgnoreTbl.begin(), keyIgnoreTbl.end(), i) != keyIgnoreTbl.end())
			{
				// 除外すべきテーブルにデータがある時
				isSelectSuccess_ = false;
				draw_ = &InputSetScene::DrawClose;
				update_ = &InputSetScene::UpdateClose;
				break;
			}
			isSelectSuccess_ = true;
			// 何らかのキーが押された時
			SetCode(static_cast<int>(i));
			return;
		}
	}

	auto mouseState = GetMouseInput();
	if (mouseState == MOUSE_INPUT_LEFT)
	{
		SetCode(-MOUSE_INPUT_LEFT);
		return;
	}
	if (mouseState == MOUSE_INPUT_RIGHT)
	{
		SetCode(-MOUSE_INPUT_RIGHT);
		return;
	}

}

void InputSetScene::UpdatePadSet(float delta, Controller& controller)
{
	auto& padState = static_cast<Pad&>(controller).GetPadState();
	auto type = static_cast<Pad&>(controller).GetPadType();
	auto& tmp{xboxPadIgnoreTbl};
	if (type == DX_PADTYPE_DUAL_SHOCK_4 || type == DX_PADTYPE_DUAL_SENSE)
	{
		tmp = psPadIgnoreTbl;
	}


	for (size_t i = 0; i < std::size(padState.Buttons); i++)
	{
		if (std::find(tmp.begin(), tmp.end(), i) != tmp.end())
		{
			// 除外すべきテーブルにデータがある時スキップする
			continue;
		}
		if (padState.Buttons[i])
		{
			SetCode(static_cast<int>(i));
			return;
		}
	}
}

void InputSetScene::UpdateClose(float delta, Controller& controller)
{
	step_ += delta;
	if (step_ >= 0.5f)
	{
		controller.Update(delta);
		ChangeSceneID(SceneID::Option);
	}
}

void InputSetScene::DrawSet(void)
{
	DrawRotaGraph(SceneManager::screenSize_<int>.x / 2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0, *mess_, true);
}

void InputSetScene::DrawClose(void)
{
	if (isSelectSuccess_)
	{
		DrawRotaGraph(SceneManager::screenSize_<int>.x / 2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0, *mess2_, true);
	}
	else
	{
		DrawRotaGraph(SceneManager::screenSize_<int>.x / 2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0, *mess3_, true);
	}
}

void InputSetScene::SetCode(int code)
{
	static_cast<OptionScene&>(*optionScene_).SetInputCode(id_, code);
	draw_ = &InputSetScene::DrawClose;
	update_ = &InputSetScene::UpdateClose;
}
