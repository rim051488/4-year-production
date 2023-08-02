#include "OptionScene.h"
#include <DxLib.h>
#include "../UI/UiManager.h"
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "../Common/SoundPross.h"
#include "InputSetScene.h"


OptionScene::OptionScene(SceneUptr owner) :
	WindowScene{std::move(owner), 0.25f, ScreenID::OptionUi,ScreenID::Option, SceneID::Option}
{
	// 現在のコンフィグの状態をセットしておく
	setResultcode_ = lpConfigMng.GetInputCode();
	onPeMono_ = lpConfigMng.GetPeConfig().at(PEID::Mono);
	onPeFog_ = lpConfigMng.GetPeConfig().at(PEID::VolFog);

	// UI生成
	SetUseASyncLoadFlag(false);
	uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/Option.ui", false);
	uiMng_->Begin(*this);

	// テクスチャをロード
	lpSceneMng.GetResourceMng().LoadTexture(frame_, "Resource/resource/Ui/OptionWindow.png");
	SetUseASyncLoadFlag(true);

	SetMakeSceneFunc(std::bind(&OptionScene::MakeInputSetFunc, this, std::placeholders::_1), SceneID::InputSet);
}

void OptionScene::SetInputCode(InputID id, int code)
{
	auto itr = std::find_if(setResultcode_.begin(), setResultcode_.end(), [code](auto& itr) { return itr.second == code; });
	if (itr != setResultcode_.end())
	{
		itr->second = setResultcode_[id];
	}
	setResultcode_[id] = code;
}

InputConfig::InputCode& OptionScene::GetInputCode(void)&
{
	return setResultcode_;
}

void OptionScene::Default(void)
{
	lpConfigMng.SetDefalutCode();
	setResultcode_ = lpConfigMng.GetInputCode();

	onPeMono_ = false;
	onPeFog_ = true;
	lpConfigMng.SetPeConfig(PEID::Mono, onPeMono_);
	lpConfigMng.SetPeConfig(PEID::VolFog, onPeFog_);
}

void OptionScene::Save(void)
{
	// 設定した結果をコンフィグに反映させる
	lpConfigMng.SetInputCode(setResultcode_);

	lpConfigMng.SetPeConfig(PEID::Mono, onPeMono_);
	lpConfigMng.SetPeConfig(PEID::VolFog, onPeFog_);
}


void OptionScene::UpdateOpend(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);
}

void OptionScene::DrawWindow(void)
{
	DrawRotaGraph(SceneManager::screenSize_<int>.x / 2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0f, *frame_, true);
	uiMng_->Draw(*screenHandle_);
}

void OptionScene::Closed(void)
{
	WindowScene::Closed();
	owner_->ChangeSceneID(owner_->GetID());

	// 止めていたループサンドを再生しなおす
	lpSooundPross.PlayBackSound(SOUNDNAME_BGM::TitleSceneBGM, true, false);
	lpSooundPross.PlayBackSound(SOUNDNAME_BGM::GameSceneStage1BGM, true, false);
	lpSooundPross.PlayBackSound(SOUNDNAME_SE::playerMove, true);
	
	ChangeSceneID(owner_->GetID());
}


BaseScene::SceneUptr OptionScene::MakeInputSetFunc(SceneUptr own)
{
	return  std::make_unique<InputSetScene>(std::move(own),setID_);
}
