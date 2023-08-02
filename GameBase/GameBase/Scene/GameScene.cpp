#include "GameScene.h"
#include <EffekseerForDXLib.h>
#include "../SceneManager.h"
#include "TitleScene.h"
#include "../Common/Camera.h"
#include "../Object/ObjectManager.h"
#include "../UI/UiManager.h"
#include "../Shader/PEManager.h"
#include "../Factory/StageFactory.h"
#include "PauseScene.h"
#include "../Common/ResourceMng.h"
#include "../Component/Transform/Transform.h"
#include "Transition/FadeLoading.h"
#include "../Common/SoundPross.h"
#include "ResultScene.h"
#include "../Common/SoundPross.h"
#include "../Component/Behavior/ThrusterBehavior.h"
#include "../Common/Input/InputConfig/InputConfig.h"
#include "../Component/Behavior/CameraBehavior.h"
#include "../Component/Behavior/StageBehavior.h"
#include "../Common/Debug.h"

GameScene::GameScene(StageID stageID) :
	BaseScene{ ScreenID::Game,SceneID::Game }, stageID_{stageID}
{
	SetMakeSceneFunc(std::bind(&GameScene::MakeResultFunc, this, std::placeholders::_1), SceneID::Result);
	SetMakeSceneFunc(std::bind(&GameScene::MakePauseFunc, this, std::placeholders::_1), SceneID::Pause);
	objMng_ = std::make_unique<ObjectManager>(10);
	peMng_ = std::make_unique<PEManager>();

	// ゲームシーン用のBGMのロード
	lpSooundPross.Init(SceneID::Game);


	// ゲームシーンのBGMの再生
	lpSooundPross.PlayBackSound(SOUNDNAME_BGM::GameSceneStage1BGM, true, false);

	if (stageID_ == StageID::Tutorial)
	{
		// チュートリアル時
		uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/game.ui", true, true, false);
		objMng_->AddFactory(std::make_unique<StageFactory>(*objMng_));
		SetMakeSceneFunc(std::bind(&GameScene::MakeSelectFunc, this, std::placeholders::_1), SceneID::Select);
	}
	else
	{
		// 通常時のゲーム時
		objMng_->AddFactory(std::make_unique<StageFactory>("Resource/Other/Stage" + std::to_string(static_cast<int>(stageID)) + ".data", *objMng_));
		uiMng_ = std::make_unique<UiManager>("Resource/Other/UiData/game.ui", true,false, false);
		lpSceneMng.GetResourceMng().MakeRenderTarget(resultCapture_, ScreenID::ResultCapture, SceneManager::screenSize_<float>, true);
	}
	AddLoadedFunc(std::bind(&GameScene::Loaded, this, std::placeholders::_1));
	result_ = ResultAttribute::Max;

	// ゲームシーンで使うシェーダをあらかじめロードしておく
	useShaders_.resize(3);
	lpSceneMng.GetResourceMng().LoadVS(useShaders_[0], "Resource/resource/Shader/Vertex/Mesh.vso");
	lpSceneMng.GetResourceMng().LoadVS(useShaders_[1], "Resource/resource/Shader/Vertex/Mesh4.vso");
	lpSceneMng.GetResourceMng().LoadPS(useShaders_[2], "Resource/resource/Shader/Pixel/Tex.pso");
	camera_->SetRotation(Quaternion(Deg2Rad(90.0f), Deg2Rad(180.0f), 0.0f));

	SetLightDirection(VGet(0.0f, -1.0f, 0.0f));
	SetLightDifColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));
	SetLightSpcColor(GetColorF(0.5f, 0.5f, 0.5f, 1.0f));
	SetLightAmbColor(GetColorF(1.0f, 1.0f, 1.0f, 1.0f));

	int x, y;
	GetDrawScreenSize(&x, &y);
	// シャドウマップを作成する際に高いフォーマット設定はいらないので設定を下げる
	// 次に作成する画像のフォーマットの設定をする
	SetDrawValidFloatTypeGraphCreateFlag(true);
	// チャネルを１にする
	SetCreateDrawValidGraphChannelNum(1);
	// ビット数を16にする
	SetCreateGraphColorBitDepth(16);
	// シャドウマップ
	shadowMap_ = MakeScreen(x, y, false);
	depth_ = MakeScreen(x, y, false);
	// 元に戻す
	SetDrawValidFloatTypeGraphCreateFlag(false);
	SetCreateDrawValidGraphChannelNum(4);
	SetCreateGraphColorBitDepth(32);
	// 被写界深度

	// オフスクリーンの初期化
	offScreen_ = MakeScreen(x, y, false);
	// スカイドームのスクリーン
	skyScreen_ = MakeScreen(x, y, false);
	// スカイドームとステージのスクリーン
	subScreen_ = MakeScreen(x, y, false);

	// シャドウマップ用カメラ情報の初期化
	SetUseASyncLoadFlag(false);
	shadowBuff_ = CreateShaderConstantBuffer(sizeof(LIGHT_MAT) * 4);
	lightMat_ = static_cast<LIGHT_MAT*>(GetBufferShaderConstantBuffer(shadowBuff_));
	lightMat.view = MGetIdent();
	lightMat.proj = MGetIdent();

	// 被写界深度用のカメラ情報の初期化
	depthbuffer_ = CreateShaderConstantBuffer(sizeof(DepthParam) * 4);
	depthMat_ = static_cast<DepthParam*>(GetBufferShaderConstantBuffer(depthbuffer_));
	depthMat.start = 0.0f;
	depthMat.end = 0.0f;
	depthMat.scope = 0.0f;
	SetUseASyncLoadFlag(true);

	SetAlwaysRunFlag(true);

#ifdef _DEBUG
	SetMakeSceneFunc(std::bind(&GameScene::MakeSelectFunc, this, std::placeholders::_1), SceneID::Select);
#endif
}

GameScene::~GameScene()
{
	DeleteShaderConstantBuffer(shadowBuff_);
	DeleteShaderConstantBuffer(depthbuffer_);
	DeleteGraph(shadowMap_);
	DeleteGraph(depth_);
}

void GameScene::Capture(void)
{
	// 一度Ui以外を描画する
	SetupShadowMap();
	SetOffsetScreen();
	SetSubScreen();
	SetUpDepth();
	depthMat_[0] = depthMat;
	peMng_->SetBuffer(depthbuffer_);
	peMng_->Draw(offScreen_, *screenHandle_, depth_, skyScreen_, subScreen_);
	// 描画結果をキャプチャする
	SetDrawScreen(*screenHandle_);
	GetDrawScreenGraph(0, 0, SceneManager::screenSize_<int>.x, SceneManager::screenSize_<int>.y, *resultCapture_);
}

void GameScene::SetUp(void)
{
	peMng_->SetFlag(PEID::Mono, lpConfigMng.GetPeConfig().at(PEID::Mono));
	peMng_->SetFlag(PEID::VolFog, lpConfigMng.GetPeConfig().at(PEID::VolFog));

	auto cam = objMng_->GetComponent<CameraBehavior>(objMng_->GetCameraID());
	if (cam.IsActive())
	{
		cam->SetSpeed(lpConfigMng.GetCameraSpeed());
	}
}

BaseScene::SceneUptr GameScene::MakeResultFunc(SceneUptr own)
{
	SetAlwaysRunFlag(false);
	Capture();

	// リザルトではループされているサウンドを止める（念のためにアフターバーナーも止める)
	lpSooundPross.SoundStop(SOUNDNAME_BGM::GameSceneStage1BGM);
	lpSooundPross.SoundStop(SOUNDNAME_SE::playerMove);
	lpSooundPross.SoundStop(SOUNDNAME_SE::playerAB);

	return std::make_unique<FadeLoading>(std::move(own), std::make_unique<ResultScene>(stageID_ ,result_, resultCapture_), 1.0f);
}

BaseScene::SceneUptr GameScene::MakePauseFunc(SceneUptr own)
{
	SetAlwaysRunFlag(false);
	return std::make_unique<PauseScene>(std::move(own));
}

BaseScene::SceneUptr GameScene::MakeSelectFunc(SceneUptr own)
{
	SetAlwaysRunFlag(false);
	return std::make_unique<FadeLoading>(std::move(own), std::make_unique<TitleScene>(), 0.5f);
}

void GameScene::Update(float delta, Controller& controller)
{
	if (controller.PressdCancel() || !GetWindowActiveFlag())
	{
		ChangeSceneID(SceneID::Pause);
		return;
	}

#ifdef _DEBUG
	if (CheckHitKey(KEY_INPUT_1))
	{
		ChangeSceneID(SceneID::Select);
		return;
	}
	if (CheckHitKey(KEY_INPUT_2))
	{
		SetResult(ResultAttribute::Clear);
		ChangeSceneID(SceneID::Result);
		return;
	}
	if (CheckHitKey(KEY_INPUT_3))
	{
		/*auto id = objMng_->GetFactory(FactoryID::HorizontalEffect).Create(ObjectID{}, {0.0f, 100.0f, 0.0f});
		objMng_->Begin(id);*/
	}
#endif
	
	auto player = (objMng_->GetComponent<Transform>(objMng_->GetPlayerID()));
	objMng_->Update(delta, controller, *this);
	uiMng_->Update(delta, *this, *objMng_, controller);
	peMng_->Update(delta);

	// エフェクシアの更新
	UpdateEffekseer3D();
}

void GameScene::DrawScene(void)
{
	// シャドウマップ作成
	SetupShadowMap();
	// オフスクリーンの作成
	SetOffsetScreen();
	// メイン以外のスクリーンの作成
	SetSubScreen();

	// 被写界深度用の深度テクスチャの作成
	SetUpDepth();

	depthMat_[0] = depthMat;
	peMng_->SetBuffer(depthbuffer_);
	// ポストエフェクトか通常描画
	peMng_->Draw(offScreen_, *screenHandle_,depth_,skyScreen_,subScreen_);

	// UIの描画
	uiMng_->Draw(*screenHandle_);
}

void GameScene::SetupShadowMap(void)
{
	// 描画先を影用深度記録画像に変更する
	SetDrawScreen(shadowMap_);
	// 影用深度記録画像を一度真っ白にする
	SetBackgroundColor(255, 255, 255);
	ClsDrawScreen();
	SetBackgroundColor(0, 0, 0);

	// シャドウマップ用にカメラをセット
	auto player = objMng_->GetComponent<Transform>(objMng_->GetPlayerID());
	if (player.IsActive())
	{
		auto& pPos = player->GetPos();
		Vector3 pTar = Vector3(pPos.x + camTar.x, camTar.y, pPos.z - camTar.z);
		camera_->SetUpShadow(offsetOrtho, offsetNear, offsetFar, pTar);
	}

	MV1SetUseOrigShader(true);
	objMng_->SetupDepthTex(*shadowPs_, -1);
	MV1SetUseOrigShader(false);

	// シャドウマップ作成に使ったカメラ情報を取得
	lightMat.view = GetCameraViewMatrix();
	lightMat.proj = GetCameraProjectionMatrix();
	// 描画用に切り替え
	SetDrawScreen(offScreen_);
	ClsDrawScreen();
	// さっき取得したカメラ情報をhlsl側に渡す
	lightMat_[0] = lightMat;
}

void GameScene::SetUpDepth(void)
{
	// 描画先を影用深度記録画像に変更
	SetDrawScreen(depth_);
	// 影用深度記録画像を一度真っ白にする
	SetBackgroundColor(255, 255, 255);
	ClsDrawScreen();
	SetBackgroundColor(0, 0, 0);
	camera_->SetGameScreen();

	// 被写界深度開始位置の計算
	depthMat.start = dofFocus - dofFocusSize / 2.0f - dofInterpSize;
	// 被写界深度終了位置を計算
	depthMat.end = dofFocus + dofFocusSize / 2.0f + dofInterpSize;
	// 被写界深度の範囲の逆数を計算
	depthMat.scope = 1.0f / (depthMat.end - depthMat.start);
	// 補間範囲とフォーカスがあっている範囲を含めた総距離を算出
	dofTotalSize_ = dofInterpSize * 2.0f + dofFocusSize;
	lightMat.eyepos = camera_->GetForward();

	// 取得したデータをhlsl側に渡す
	depthMat_[0] = depthMat;
	MV1SetUseOrigShader(true);
	objMng_->SetupDepthTex(*depthPS_, depthbuffer_);
	MV1SetUseOrigShader(false);

	// 描画用に切り替え
	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
	// 被写界深度開始位置の計算
	depthMat.start = dofInterpSize / dofTotalSize_;
	// 被写界深度終了位置の計算
	depthMat.end = (dofInterpSize + dofFocusSize) / dofTotalSize_;

}

void GameScene::SetOffsetScreen(void)
{
	//camera_->SetPos(Vector3(0.0f, 400.0f, 0.0f));
	camera_->SetGameScreen();	

	// 描画
	objMng_->ShadowDraw(shadowMap_,shadowBuff_);
	MV1SetUseOrigShader(false);

	// エフェクシアの描画
	Effekseer_Sync3DSetting();
	DrawEffekseer3D();

	// 3D系のデバッグ描画
	DebugDraw3DScreen();

	SetDrawScreen(*screenHandle_);
	ClsDrawScreen();
}

void GameScene::SetSubScreen(void)
{
	// スカイドームのみのスクリーン
	SetDrawScreen(skyScreen_);
	ClsDrawScreen();
	camera_->SetGameScreen();
	auto [result, id] = objMng_->Find(ObjectAttribute::Sky);
	objMng_->GetComponent<Render>(id)->Draw();
	
	// スカイドームとステージのみのスクリーン
	SetDrawScreen(subScreen_);
	ClsDrawScreen();
	camera_->SetGameScreen();
	auto [result1, id1] = objMng_->Find(ObjectAttribute::Stage);
	objMng_->GetComponent<Render>(id)->Draw();
	objMng_->GetComponent<Render>(id1)->Draw();
}

bool GameScene::IsLoaded(void)
{
	return BaseScene::IsLoaded() && objMng_->IsLoaded() && uiMng_->IsLoaded();
}

void GameScene::Loaded(Controller& controller)
{
	// ロード完了時に呼ばれる
	lpSceneMng.GetResourceMng().Loaded();
	objMng_->Begin(); 
	objMng_->Update(0.0f, controller, *this);
	uiMng_->Begin(*this);

	
	lpSceneMng.GetResourceMng().LoadPS(shadowPs_, "Resource/resource/Shader/ShadowMap/ShadowMap.pso");
	lpSceneMng.GetResourceMng().LoadPS(depthPS_, "Resource/resource/Shader/PostEffect/Dof/depth.pso");
	SetUp();
	UpdateEffekseer3D();
}

void GameScene::Back(void)
{
	SetAlwaysRunFlag(true);

	DebugLog("SE", lpSooundPross.GetSEVolumeUserSet());
}
