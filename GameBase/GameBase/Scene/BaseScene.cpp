#include <DxLib.h>
#include "BaseScene.h"
#include "../SceneManager.h"
#include "../Object/ObjectManager.h"
#include "../Common/ResourceMng.h"
#include "../Common/Camera.h"
#include "../UI/UiManager.h"
#include "../Shader/PEManager.h"
#include "../Common/Input/InputType/Keyboard.h"
#include "../Common/Input/InputType/Pad.h"
#include "../Common/Debug.h"
BaseScene::BaseScene(ScreenID id, SceneID sceneID) :
	scID_{ id }, nextID_{ sceneID }
{
	// スクリーン作成
	lpSceneMng.GetResourceMng().MakeRenderTarget(screenHandle_,scID_, lpSceneMng.screenSize_<float>, true);
	
	// カメラの作成
	camera_ = std::make_unique<Camera>();
}

BaseScene::BaseScene(std::unique_ptr<ObjectManager>&& objectManager, ScreenID id, SceneID sceneID) :
	scID_{ id }, nextID_{ sceneID }, objMng_{std::move(objectManager)}
{
	// スクリーン作成
	lpSceneMng.GetResourceMng().MakeRenderTarget(screenHandle_, scID_, lpSceneMng.screenSize_<float>, true);

	// カメラの作成
	camera_ = std::make_unique<Camera>();
}

BaseScene::~BaseScene()
{
}


void BaseScene::Draw(void)
{
	DrawGraph(0, 0, *screenHandle_,true);
}

void BaseScene::Relese(void)
{
}

bool BaseScene::IsLoaded(void)
{
	return GetASyncLoadNum() <= 0;
}

void BaseScene::Back(void)
{
	// デフォルトは何もしない
}

BaseScene::SceneUptr BaseScene::Update(SceneUptr own, float delta, Controller& controller)
{
	Update(delta, controller);
	if (nextID_ != GetID())
	{
		
		// 次のシーンのIDと一致してないとき
		if (sceneMakeFuncMap_.contains(nextID_))
		{
			// 作成用functionがあるときそれを実行する
			return sceneMakeFuncMap_[nextID_](std::move(own));
		}
	}
	return own;
}

void BaseScene::ChangeSceneID(SceneID id)
{
	nextID_ = id;
}



void BaseScene::SetMakeSceneFunc(std::function<SceneUptr(SceneUptr)>&& sceneMakeFunc, const SceneID id)
{
	sceneMakeFuncMap_.emplace(id, std::move(sceneMakeFunc));
}

void BaseScene::Loaded(Controller& controller)
{
	// ロード終了したのでリストの中身をすべて実行する
	std::for_each(loadedFunc_.begin(), loadedFunc_.end(), [&controller](LoadedFunc& f) { f(controller); });

	// もう使わないのでクリアしとく
	loadedFunc_.clear();
}

void BaseScene::AddLoadedFunc(LoadedFunc&& loadedFunc)
{
	loadedFunc_.emplace_back(std::move(loadedFunc));
}





