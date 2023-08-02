#include <DxLib.h>
#include "TitleScene.h"
#include "Transition/BackLoading.h"
#include "../SceneManager.h"
#include "../Shader/PEManager.h"
#include "../Shader/PEID.h"
#include "../Common/Camera.h"
#include "../Common/ResourceMng.h"
#include "../Common/SoundPross.h"
#include "SelectScene.h"
#include "../UI/UiManager.h"
#include "../Object/ObjectManager.h"
#include "../Component/Render/StageRender.h"
#include "../Component/Render/ModelRender.h"
#include "../Component/Render/NonLightingRender.h"
#include "../Component/Transform/Transform.h"
#include "../Component/Info/ObjectInfo.h"
#include "../Component/Behavior/CameraBehavior.h"
#include "../Common/Debug.h"

constexpr float logoUiOffTIme{ 0.5f };

TitleScene::TitleScene() :
	BaseScene{ScreenID::Title,SceneID::Title},
	update_{&TitleScene::UpdateNon}, draw_{&TitleScene::DrawNon}, stepTime_{0.0f}
{
	SetMakeSceneFunc(std::bind(&TitleScene::MakeGameFunc, this, std::placeholders::_1), SceneID::Select);
	
	peMng_ = std::make_unique<PEManager>();
	
	uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/title.ui", true, false, false);

	CreateBackGround();

	AddLoadedFunc(std::bind(&TitleScene::Loaded, this, std::placeholders::_1));

	// タイトルシーン用にロードしたサウンドを呼びだす
	lpSooundPross.Init(SceneID::Title);

	// タイトルシーンのBGMを再生
	lpSooundPross.PlayBackSound(SOUNDNAME_BGM::TitleSceneBGM,true,false);
}

TitleScene::~TitleScene()
{
	MV1DeleteModel(skyDomeModel_);
}

BaseScene::SceneUptr TitleScene::MakeGameFunc(SceneUptr own)
{
	return std::make_unique<BackLoading>(std::move(own), std::make_unique<SelectScene>(std::move(objMng_)), 2.0f);
}

void TitleScene::Update(float delta, Controller& controller)
{
	(this->*update_)(delta, controller);
}

void TitleScene::DrawScene(void)
{
	cubeDraw();
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	(this->*draw_)();
	MV1SetUseOrigShader(false);
}

bool TitleScene::IsLoaded(void)
{
	return BaseScene::IsLoaded() && uiMng_->IsLoaded();
}

void TitleScene::UpdateNon(float delta, Controller& controller)
{
	objMng_->Update(delta, controller, *this);
	uiMng_->Update(delta, *this, *objMng_, controller);
	if (CheckHitKeyAll())
	{
		update_ = &TitleScene::UpdateLogoOff;
		draw_ = &TitleScene::DrawLogoOff;
	}
}

void TitleScene::UpdateLogoOff(float delta, Controller& controller)
{
	if ((stepTime_ += delta) >= logoUiOffTIme)
	{
		ChangeSceneID(SceneID::Select);
	}
}

void TitleScene::DrawLogoOff(void)
{
	camera_->SetUpScreen();
	objMng_->ShadowDraw(-1,-1, dynamicCubeTex_);
	auto alpha =1.0f - stepTime_ / logoUiOffTIme;
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>(255.0f * alpha));
	uiMng_->Draw(*screenHandle_);
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
}

void TitleScene::DrawNon(void)
{
	camera_->SetUpScreen();
	camera_->SetRotation(Quaternion(Deg2Rad(90.0f), Deg2Rad(180.0f), 0.0f));
	camera_->SetPos(Vector3(0.0f, 500.0f, -500.0f));
	objMng_->ShadowDraw(-1,-1, dynamicCubeTex_);
	uiMng_->Draw(*screenHandle_);
}

void TitleScene::CreateBackGround(void)
{
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

}

void TitleScene::Loaded(Controller& controller)
{
	lpSceneMng.GetResourceMng().Loaded();
	objMng_->Begin();
	objMng_->Update(0.0f, controller, *this);
	uiMng_->Begin(*this);
}

void TitleScene::cubeDraw(void)
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

