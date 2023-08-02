#pragma once
#include <DxLib.h>
#include "BaseScene.h"
#include <vector>
#include "../Common/SharedHandle.h"
#include "StageID.h"

struct DepthParam
{
	float start;
	float end;
	float scope;
};

struct WEIGHT_NUM
{
	float weight[8];
};

// 敵との距離と角度を確保
struct MiniMapRadar
{
	float angle;
	float range;
	bool flag;
};

class GameScene :
    public BaseScene
{
public:
	GameScene(StageID stageID);
	~GameScene();

	// ライトカメラの位置
	static constexpr Vector3 camPos{ 500.0f, 800.0f, -3000.0f };
	// ライトカメラの注視点
	static constexpr Vector3 camTar{ 0.0f, 0.0f, -1000.0f };
	// ライトカメラの正射影の表示範囲
	static constexpr float offsetOrtho = 22000.0f;
	// ライトカメラの手前の距離と奥の距離
	static constexpr float offsetNear = 0.001f;
	static constexpr float offsetFar = 23000.0f;
	// ガウシアンブラの重さ
	static constexpr int NUM_WEIGHTS = 8;

	// ぼかし無しとぼかしありの補間距離
	static constexpr float dofInterpSize = 7000;
	// ぼかし無しの範囲の中心位置
	static constexpr float dofFocus = 500.0;
	// ぼかし無しの範囲
	static constexpr float dofFocusSize = 27000;

	// ミニマップに敵を表示する範囲
	static constexpr float RADAR_RANGE = 90;

	void SetResult(ResultAttribute result)
	{
		result_ = result;
	}

	/// <summary>
	/// 画面をキャプチャする
	/// </summary>
	/// <param name=""></param>
	void Capture(void);

	/// <summary>
	/// 開始時やポーズ画面からの復帰後に行う処理
	/// </summary>
	/// <param name=""></param>
	void SetUp(void);

private:
	const SceneID GetID(void) const
	{
		return SceneID::Game;
	}

	/// <summary>
	/// リザルトシーンへの生成関数
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakeResultFunc(SceneUptr own);

	/// <summary>
	/// ポーズシーン生成関数
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakePauseFunc(SceneUptr own);

	/// <summary>
	/// セレクトシーン生成関数
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakeSelectFunc(SceneUptr own);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="delta"></param>
	void Update(float delta, Controller& controller) final;

	/// <summary>
	/// シーンの描画
	/// </summary>
	/// <param name=""></param>
	void DrawScene(void) final;
	/// <summary>
	/// シャドウマップの作成
	/// </summary>
	/// <param name=""></param>
	void SetupShadowMap(void);
	/// <summary>
	/// 被写界深度用の深度テクスチャ
	/// </summary>
	/// <param name=""></param>
	void SetUpDepth(void);
	/// <summary>
	/// ポストエフェクトで使うスクリーンの作成
	/// </summary>
	/// <param name=""></param>
	void SetOffsetScreen(void);

	/// <summary>
	/// メイン以外のスクリーンの作成
	/// </summary>
	/// <param name=""></param>
	void SetSubScreen(void);

	/// <summary>
	/// ロード完了しているかを返す
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool IsLoaded(void) final;

	/// <summary>
	/// ロード完了時の処理
	/// </summary>
	/// <param name=""></param>
	void Loaded(Controller& controller);

	void Back(void) final;

	// シャドウマップ用の深度テクスチャ
	// シャドウマップ用シェーダ
	SharedShaderHandle shadowPs_;
	int shadowMap_;
	LIGHT_MAT* lightMat_;
	LIGHT_MAT lightMat;
	int shadowBuff_;

	// 被写界深度用の深度テクスチャ
	// 深度テクスチャ用シェーダ
	SharedShaderHandle depthPS_;
	int depth_;
	DepthParam* depthMat_;
	DepthParam depthMat;
	int depthbuffer_;

	float dofTotalSize_;

	// オフスクリーン
	int offScreen_;

	// スカイドームのみのスクリーン
	int skyScreen_;
	// スカイドームとステージのみのスクリーン
	int subScreen_;

	// 使用する頂点シェーダ
	std::vector<SharedShaderHandle> useShaders_;

	ResultAttribute result_;

	// リザルトシーンで使うゲーム終了時のキャプチャの保存先
	SharedRenderTargetHandle resultCapture_;

	// 現在のステージID
	StageID stageID_;

	friend class PauseScene;
};

