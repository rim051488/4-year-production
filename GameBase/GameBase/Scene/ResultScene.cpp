#include <DxLib.h>
#include "ResultScene.h"
#include "Transition/FadeLoading.h"
#include "GameScene.h"
#include "SelectScene.h"
#include "../SceneManager.h"
#include "../Common/Camera.h"
#include "../Common/ResourceMng.h"
#include "../Common/SoundPross.h"
#include "../UI/UiManager.h"
#include "../Shader/PEManager.h"
#include "DialogScene.h"

#include "../Common/Debug.h"

#include <type_traits>

ResultScene::ResultScene(StageID stageID, ResultAttribute atr, SharedRenderTargetHandle& capture) :
	BaseScene{ ScreenID::Result,SceneID::Result }, capture_{ capture }, stageID_{stageID}
{
	SetMakeSceneFunc(std::bind(&ResultScene::MakeSelectFunc, this, std::placeholders::_1), SceneID::Select);
	SetMakeSceneFunc(std::bind(&ResultScene::MakeRetryFunc, this, std::placeholders::_1), SceneID::Game);
	SetMakeSceneFunc(std::bind(&ResultScene::MakeDialogFunc, this, std::placeholders::_1), SceneID::Dialog);
	peMng_ = std::make_unique<PEManager>();

	lpSooundPross.Init(SceneID::Result);

	if (atr == ResultAttribute::Clear)
	{
		if (static_cast<StageID>(std::underlying_type <StageID>::type(stageID_) + 1) == StageID::Max)
		{
			uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/Clear.ui");
		}
		else
		{
			uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/ClearNext.ui");
		}
		peMng_->SetFlag(PEID::Default);
		lpSooundPross.PlayBackSound(SOUNDNAME_BGM::ResultClearBGM, true, true);
	}
	else
	{
		uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/GameOver.ui");
		peMng_->SetFlag(PEID::CtoM);
		lpSooundPross.PlayBackSound(SOUNDNAME_BGM::ResultFailBGM, true, true);
	}
	
	lpSceneMng.GetResourceMng().LoadTexture(frame_, "./Resource/resource/ResultFrame.png");

	AddLoadedFunc([this](auto& contr) { 
		uiMng_->Begin(*this); 
		});
}

void ResultScene::NextStage(void)
{
	stageID_ = static_cast<StageID>(std::underlying_type <StageID>::type(stageID_) + 1);
	ChangeSceneID(SceneID::Game);
}

BaseScene::SceneUptr ResultScene::MakeSelectFunc(SceneUptr own)
{
	lpSooundPross.Release();
	return std::make_unique<FadeLoading>(std::move(own), std::make_unique<SelectScene>(), 2.0f);
}

BaseScene::SceneUptr ResultScene::MakeRetryFunc(SceneUptr own)
{
	lpSooundPross.Release();
	return std::make_unique<FadeLoading>(std::move(own), std::make_unique<GameScene>(stageID_), 2.0f);
}

BaseScene::SceneUptr ResultScene::MakeDialogFunc(SceneUptr own)
{
	cursorPos_ = lpSceneMng.GetController().GetCursorPos();
	return std::make_unique<DialogScene>(std::move(own));
}

void ResultScene::Update(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);
	peMng_->Update(delta);
}

void ResultScene::DrawScene(void)
{
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	peMng_->Draw(*capture_, *screenHandle_, -1, -1, -1);
	DrawGraph(0, 0, *frame_, true);
	uiMng_->Draw(*screenHandle_);
}

bool ResultScene::IsLoaded(void)
{
	return BaseScene::IsLoaded() && uiMng_->IsLoaded();
}

void ResultScene::Back(void)
{
	lpSceneMng.GetController().SetCursorPos(cursorPos_);
	lpSooundPross.Release();
}
