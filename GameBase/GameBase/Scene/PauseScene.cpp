#include <DxLib.h>
#include "PauseScene.h"
#include "TitleScene.h"
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "OptionScene.h"
#include "../UI/UiManager.h"
#include "DialogScene.h"
#include "../Common/SoundPross.h"
#include "GameScene.h"
#include "../Shader/PEManager.h"

// ÇøÇÂÇ§Ç«Ç¢Ç¢ä¥Ç∂Ç…í≤êÆÇ∑ÇÈÇ±Ç∆
constexpr Vector2I pauseWndSize{ 1280,720 };

PauseScene::PauseScene(SceneUptr owner):
	WindowScene{std::move(owner),0.25f,ScreenID::PauseUi, ScreenID::Pause,SceneID::Pause}
{
	
	choiceNextID_ = owner_->GetID();
	
	SetMakeSceneFunc(std::bind(&PauseScene::MakeDialogFunc, this, std::placeholders::_1),SceneID::Dialog);
	SetMakeSceneFunc(std::bind(&PauseScene::MakeOptionFunc, this, std::placeholders::_1), SceneID::Option);
	SetUseASyncLoadFlag(false);
	uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/pause.ui",false);
	uiMng_->Begin(*this);
	lpSceneMng.GetResourceMng().LoadTexture(frame_, "Resource/resource/PauseFrame.png");
	SetUseASyncLoadFlag(true);
}

void PauseScene::BackGame(void)
{
	choiceNextID_ = owner_->GetID();
	Close();
}

void PauseScene::Dialog(void)
{
	cursorPos_ = lpSceneMng.GetController().GetCursorPos();
	choiceNextID_ = SceneID::Dialog;
	Close();
}

void PauseScene::Option(void)
{
	cursorPos_ = lpSceneMng.GetController().GetCursorPos();
	choiceNextID_ = SceneID::Option;
	lpSooundPross.SoundStop(SOUNDNAME_BGM::GameSceneStage1BGM);
	lpSooundPross.SoundStop(SOUNDNAME_SE::playerMove);
	Close();
}

BaseScene::SceneUptr PauseScene::MakeGameFunc(SceneUptr own)
{
	return std::make_unique<TitleScene>();
}

BaseScene::SceneUptr PauseScene::MakeDialogFunc(SceneUptr own)
{
	return std::make_unique<DialogScene>(std::move(own));
}

BaseScene::SceneUptr PauseScene::MakeOptionFunc(SceneUptr own)
{
	return std::make_unique<OptionScene>(std::move(own));
}

void PauseScene::UpdateOpend(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);
}

void PauseScene::DrawWindow(void)
{
	DrawRotaGraph(SceneManager::screenSize_<int>.x / 2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0f, *frame_, true);
	uiMng_->Draw(*screenHandle_);
}

void PauseScene::Closed(void)
{
	WindowScene::Closed();
	Open();
	ChangeSceneID(choiceNextID_);

	// å„Ç≈PEMngÇ…çáÇÌÇπÇƒèëÇ´ä∑Ç¶ÇÈ
	if (choiceNextID_ == SceneID::Game)
	{
		auto& gameScene = static_cast<GameScene&>(*owner_);
		gameScene.SetUp();
	}
}

void PauseScene::Back(void)
{
	lpSceneMng.GetController().SetCursorPos(cursorPos_);
}


