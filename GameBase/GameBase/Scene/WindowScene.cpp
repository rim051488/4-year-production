#include "WindowScene.h"
#include "../Common/ResourceMng.h"
#include "../UI/UiManager.h"
#include "../UI/UiBase.h"

WindowScene::WindowScene(SceneUptr owner, float actTime, ScreenID wndScreenID, ScreenID screenID, SceneID sceneID) :
	step_{0.0f}, actTime_{ actTime }, owner_{std::move(owner)},
	update_{&WindowScene::UpdateOpen}, draw_{&WindowScene::DrawOpen},
	BaseScene{screenID, sceneID}
{
	SetUseASyncLoadFlag(false);
	lpSceneMng.GetResourceMng().MakeRenderTarget(window_, wndScreenID, SceneManager::screenSize_<float>, true);
	SetUseASyncLoadFlag(true);

	if (owner_)
	{
		// 呼び出し主が存在するとき呼び出し主に戻るための処理を追加しとく
		SetMakeSceneFunc([this](auto own) { 
			owner_->ChangeSceneID(owner_->GetID()); return std::move(owner_);
			}, owner_->GetID());
		
		// 呼び出し主がカーソルを表示しているかを見る
		auto cursor = std::find_if(owner_->GetUiManager().GetUiList().begin(), owner_->GetUiManager().GetUiList().end(), [](auto& ui) { return ui->GetUIID() == UiID::Cursor; });
		if (cursor != owner_->GetUiManager().GetUiList().end())
		{
			// カーソルがある時表示をやめる
			(*cursor)->SetDrawFlag(false);
		}
		owner_->DrawScene();
	}
}


void WindowScene::Open(void)
{
	update_ = &WindowScene::UpdateOpen;
	draw_ = &WindowScene::DrawOpen;
	step_ = 0.0f;
}

void WindowScene::Close(void)
{
	update_ = &WindowScene::UpdateClose;
	draw_ = &WindowScene::DrawClose;
	step_ = 0.0f;
}

void WindowScene::Update(float delta, Controller& controller)
{
	(this->*update_)(delta, controller);
}

void WindowScene::Closed(void)
{
	step_ = actTime_;
	if (owner_)
	{
		owner_->Back();
		// 呼び出し主がカーソルを表示しているか調べる
		auto cursor = std::find_if(owner_->GetUiManager().GetUiList().begin(), owner_->GetUiManager().GetUiList().end(), [](auto& ui) { return ui->GetUIID() == UiID::Cursor; });
		if (cursor != owner_->GetUiManager().GetUiList().end())
		{
			// カーソルを表示しているとき表示をオンにする
			(*cursor)->SetDrawFlag(true);
		}
	}
}

void WindowScene::DrawBackGround(void)
{
	if (owner_)
	{
		// 呼び出し主が存在する場合は描画する
		owner_->Draw();
	}
}

void WindowScene::DrawScene(void)
{
	SetDrawScreen(*window_);
	ClsDrawScreen();
	DrawWindow();


	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	DrawBackGround();
	(this->*draw_)();
}

void WindowScene::UpdateOpen(float delta, Controller& controller)
{
	step_ += delta;
	if (step_ >= actTime_)
	{
		update_ = &WindowScene::UpdateOpend;
		draw_ = &WindowScene::DrawOpend;
		step_ = 0.0f;
	}
}

void WindowScene::UpdateClose(float delta, Controller& controller)
{
	step_ += delta;
	if (step_ >= actTime_)
	{
		Closed();
	}
}

void WindowScene::DrawOpen(void)
{
	constexpr float halfY = (SceneManager::screenSize_<float>.y / 2.0f);
	float yup = halfY * (1.0f - step_ / actTime_);
	float ybtm = halfY + halfY * step_ / actTime_;
	DrawModiGraphF(
		0.0f, yup,
		SceneManager::screenSize_<float>.x, yup,
		SceneManager::screenSize_ <float>.x, ybtm,
		0.0f, ybtm,
		*window_, true
	);
}

void WindowScene::DrawClose(void)
{
	constexpr float halfY = (SceneManager::screenSize_<float>.y / 2.0f);
	float yup = halfY * step_ / actTime_;
	float ybtm = halfY + halfY * (1.0f - step_ / actTime_);
	DrawModiGraphF(
		0.0f, yup,
		SceneManager::screenSize_<float>.x, yup,
		SceneManager::screenSize_ <float>.x, ybtm,
		0.0f, ybtm,
		*window_, true
	);
}

void WindowScene::DrawOpend(void)
{
	DrawGraph(0, 0, *window_, true);
}
