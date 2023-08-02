#include "SelectScene.h"
#include "Transition/Loading.h"
#include "GameScene.h"
#include "OptionScene.h"
#include "../UI/UiManager.h"
#include "Transition/FadeLoading.h"
#include "DialogScene.h"
#include "../Common/ResourceMng.h"
#include "../Common/SoundPross.h"
#include "../Object/ObjectManager.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Render/StageRender.h"
#include "../Component/Render/NonLightingRender.h"
#include "../Component/Transform/Transform.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Common/Camera.h"

#ifdef _DEBUG
#include "TitleScene.h"
#endif

SelectScene::SelectScene()
	: WindowScene{nullptr, 0.25f , ScreenID::SelectUi,ScreenID::Select,SceneID::Select }
{
	Init();
	objMng_ = std::make_unique<ObjectManager>(1ull);
	
	camera_->SetRotation(Quaternion(Deg2Rad(90.0f), Deg2Rad(180.0f), 0.0f));
	camera_->SetPos(Vector3(0.0f, 600.0f, -500.0f));
	// スカイドーム
	auto id = objMng_->MakeObjectID();
	auto skyRender = std::make_unique<NonLightingRender>();
	static_cast<Render&>(*skyRender).Load("Resource/ModelData/stage/SkyDome.mv1");
	objMng_->AddComponent(std::move(skyRender), id);
	auto skyTransform = std::make_unique<Transform>();
	skyTransform->Scale() = Vector3{ 100.0f,100.0f,100.0f };
	skyTransform->Pos() = Vector3{ 0,0,0 };
	objMng_->AddComponent(std::move(skyTransform), id);
	objMng_->AddComponent(std::make_unique<ObjectInfo>(), id);

	// ステージモデル
	id = objMng_->MakeObjectID();
	auto render = std::make_unique<StageRender>();
	static_cast<Render&>(*render).Load("Resource/ModelData/stage/water10.mv1");
	//render->SetNonShader();
	objMng_->AddComponent(std::move(render), id);
	auto transform = std::make_unique<Transform>();
	transform->Scale() = Vector3{ 10.0f,10.0f,10.0f };
	objMng_->AddComponent(std::move(transform), id);
	objMng_->AddComponent(std::make_unique<ObjectInfo>(), id);

	skyDomeModel_ = MV1LoadModel(L"Resource/ModelData/stage/SkyDome.mv1");
	MV1SetScale(skyDomeModel_, VGet(30, 30, 30));

	// キューブマップテクスチャ
	SetCubeMapTextureCreateFlag(true);
	dynamicCubeTex_ = MakeScreen(cubeSize, cubeNum, true);
	SetCubeMapTextureCreateFlag(false);
	//camera_->SetRotation(Quaternion(0.0f, Deg2Rad(180.0f), 0.0f));
	SetLightDirection(VGet(0.0f, -1.0f, 0.0f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightSpcColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
	AddLoadedFunc(std::bind(&SelectScene::Loaded, this, std::placeholders::_1));

}

SelectScene::SelectScene(std::unique_ptr<ObjectManager>&& objectManager) :
	WindowScene{ nullptr, 0.25f , ScreenID::SelectUi,ScreenID::Select,SceneID::Select }
{
	objMng_ = std::move(objectManager);
	Init();
}

void SelectScene::Dialog(void)
{
	choiceNextID_ = SceneID::Dialog;
	cursorPos_ = lpSceneMng.GetController().GetCursorPos();
	Close();
}

void SelectScene::Option(void)
{
	choiceNextID_ = SceneID::Option;
	cursorPos_ = lpSceneMng.GetController().GetCursorPos();

	// タイトルシーンのBGMの一時停止
	lpSooundPross.SoundStop(SOUNDNAME_BGM::TitleSceneBGM);

	Close();

	//
}

void SelectScene::Play(void)
{
	choiceNextID_ = SceneID::Game;
	lpSooundPross.Release(SOUNDNAME_BGM::TitleSceneBGM);
	Close();
}

void SelectScene::Tutorial(void)
{
	staegID_ = StageID::Tutorial;
	choiceNextID_ = SceneID::Game;
	lpSooundPross.Release(SOUNDNAME_BGM::TitleSceneBGM);
	Close();
}

BaseScene::SceneUptr SelectScene::MakeGameFunc(SceneUptr own)
{
	return std::make_unique<FadeLoading>(std::move(own), std::make_unique<GameScene>(staegID_), 2.0f);
}

BaseScene::SceneUptr SelectScene::MakeOptionFunc(SceneUptr own)
{
	return std::make_unique<OptionScene>(std::move(own));
}

BaseScene::SceneUptr SelectScene::MakeDialogFunc(SceneUptr own)
{
	return std::make_unique<DialogScene>(std::move(own));
}

#ifdef _DEBUG

BaseScene::SceneUptr SelectScene::MakeTitleFunc(SceneUptr own)
{
	return std::make_unique<FadeLoading>(std::move(own), std::make_unique<TitleScene>(), 1.0f);
}

#endif

void SelectScene::UpdateOpend(float delta, Controller& controller)
{
	uiMng_->Update(delta, *this, *objMng_, controller);

#ifdef _DEBUG
	if (CheckHitKey(KEY_INPUT_1))
	{
		ChangeSceneID(SceneID::Title);
	}
#endif
}

void SelectScene::DrawWindow(void)
{
	DrawRotaGraph(SceneManager::screenSize_<int>.x / 2, SceneManager::screenSize_<int>.y / 2, 1.0, 0.0f, *frame_, true);
	uiMng_->Draw(*screenHandle_);
}

void SelectScene::Closed(void)
{
	WindowScene::Closed();
	DrawScene();
	Open();
	ChangeSceneID(choiceNextID_);
}

bool SelectScene::IsLoaded(void)
{
	return BaseScene::IsLoaded() && uiMng_->IsLoaded();
}

void SelectScene::Init(void)
{
	SetMakeSceneFunc(std::bind(&SelectScene::MakeGameFunc, this, std::placeholders::_1), SceneID::Game);
	SetMakeSceneFunc(std::bind(&SelectScene::MakeDialogFunc, this, std::placeholders::_1), SceneID::Dialog);
	SetMakeSceneFunc(std::bind(&SelectScene::MakeOptionFunc, this, std::placeholders::_1), SceneID::Option);

#ifdef _DEBUG
	SetMakeSceneFunc(std::bind(&SelectScene::MakeTitleFunc, this, std::placeholders::_1), SceneID::Title);
#endif

	uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/select.ui");
	AddLoadedFunc([this](auto&) { uiMng_->Begin(*this); });
	lpSceneMng.GetResourceMng().LoadTexture(frame_, "Resource/resource/SelectFrame.png");
	staegID_ = StageID::Stage1;
	camera_->SetRotation(Quaternion(0.0f, Deg2Rad(180.0f), 0.0f));
	camera_->SetPos(Vector3(0.0f, 200.0f, 200.0f));
	camera_->SetRotation(Quaternion(0, Deg2Rad(180.0f), 0.0f));
	camera_->SetPos(Vector3(0.0f, 1000.0f, -100.0f));

	SetLightDirection(VGet(0.0f, -1.0f, 0.0f));
	SetLightDifColor(GetColorF(1.f, 1.f, 1.f, 1.f));
	SetLightSpcColor(GetColorF(0.5f, 0.5f, 0.5f, 0.5f));
	SetLightAmbColor(GetColorF(0.5f, 0.5f, 0.5f, 1.f));

	// セレクトシーンのサウンド呼び出し
	lpSooundPross.Init(SceneID::Select);
	// サウンドの再生
	lpSooundPross.PlayBackSound(SOUNDNAME_SE::openMenu,false);
}

void SelectScene::DrawBackGround(void)
{
	camera_->SetUpScreen();
	objMng_->ShadowDraw(shadowMap_, shadowBuff_);
}

void SelectScene::Back(void)
{
	lpSceneMng.GetController().SetCursorPos(cursorPos_);
}

void SelectScene::Loaded(Controller& controller)
{
	lpSceneMng.GetResourceMng().Loaded();
	objMng_->Begin();
	objMng_->Update(0.0f, controller, *this);
}

void SelectScene::SetupShadowMap(void)
{
	camera_->SetUpScreen();
	camera_->SetRotation(Quaternion(Deg2Rad(90.0f), Deg2Rad(180.0f), 0.0f));
	camera_->SetPos(Vector3(0.0f, 500.0f, -500.0f));
	objMng_->ShadowDraw(-1, -1, dynamicCubeTex_);
	uiMng_->Draw(*screenHandle_);

}

void SelectScene::cubeDraw(void)
{
	// 環境を描画する面の数
	for (int i = 0; i < cubeNum; i++)
	{
		// 描画先をレンダーターゲットに設定
		SetRenderTargetToShader(0, dynamicCubeTex_, i);
		ClearDrawScreen();
		camera_->SetRotation(Quaternion(0, 0.0f, 0.0f));
		camera_->SetPos(Vector3(0.0f, 2000.0f, -500.0f));

		// モデルの描画
		MV1DrawModel(skyDomeModel_);
	}
}

