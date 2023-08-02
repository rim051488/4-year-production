#include "DialogScene.h"
#include <DxLib.h>
#include "../SceneManager.h"
#include "../Common/ResourceMng.h"
#include "../UI/UiManager.h"


DialogScene::DialogScene(SceneUptr owner) :
	WindowScene{std::move(owner),0.25f, ScreenID::DialogUi, ScreenID::Dialog, SceneID::Dialog}
{
	
	SetUseASyncLoadFlag(false);
	uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/dialog.ui", false);
	uiMng_->Begin(*this);
	lpSceneMng.GetResourceMng().LoadTexture(frame_, L"Resource/resource/DialogFrame.png");
	SetUseASyncLoadFlag(true);
}

void DialogScene::Ok(void)
{
	lpSceneMng.End();
}

void DialogScene::Cancel(void)
{
	Close();
}

void DialogScene::UpdateOpend(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);
}

void DialogScene::DrawWindow(void)
{
	DrawRotaGraph(SceneManager::screenSize_<int>.x/2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0f, *frame_, true);
	uiMng_->Draw(*screenHandle_);
}

void DialogScene::Closed(void)
{
	WindowScene::Closed();
	auto* ownerPtr = dynamic_cast<WindowScene*>(&*owner_);
	if (ownerPtr != nullptr)
	{
		ownerPtr->Open();
	}

	owner_->ChangeSceneID(owner_->GetID());
	ChangeSceneID(owner_->GetID());
}


