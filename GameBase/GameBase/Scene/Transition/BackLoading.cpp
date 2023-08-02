#include "BackLoading.h"
#include "../../UI/UiManager.h"

BackLoading::BackLoading(SceneUptr befor, SceneUptr after, float transTime) :
	Loading{std::move(befor),std::move(after), transTime}
{
}

void BackLoading::Update(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);
	if (after_->IsLoaded())
	{
		after_->Loaded(controller);
		after_->DrawScene();
		ChangeSceneID(after_->GetID());
	}
}

void BackLoading::DrawScene(void)
{
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	befor_->Draw();
	uiMng_->Draw(*screenHandle_);

}
