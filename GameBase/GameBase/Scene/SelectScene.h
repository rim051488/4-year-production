#pragma once
#include "WindowScene.h"
#include "StageID.h"

// セレクトシーン
class SelectScene :
    public WindowScene
{
public:
	SelectScene();
	// ライトカメラの注視点
	static constexpr Vector3 camTar{ 0.0f, 0.0f, -0 };
	// ライトカメラの正射影の表示範囲
	static constexpr float offsetOrtho = 1000.0f;
	// ライトカメラの手前の距離と奥の距離
	static constexpr float offsetNear = 0.001f;
	static constexpr float offsetFar = 2000.0f;

	// キューブマップのサイズ
	static constexpr int cubeSize = 1024;
	// キューブマップの数
	static constexpr int cubeNum = 6;


	SelectScene(std::unique_ptr<ObjectManager>&& objectManager);

	/// <summary>
	/// 終了へ(ダイアログ)
	/// </summary>
	/// <param name=""></param>
	void Dialog(void);

	/// <summary>
	/// オプションシーンへ
	/// </summary>
	/// <param name=""></param>
	void Option(void);

	/// <summary>
	/// ゲームシーンへ
	/// </summary>
	/// <param name=""></param>
	void Play(void);

	/// <summary>
	/// チュートリアルへ
	/// </summary>
	/// <param name=""></param>
	void Tutorial(void);
private:
	const SceneID GetID(void) const
	{
		return SceneID::Select;
	}

	// 新しいゲームシーンへの生成関数
	SceneUptr MakeGameFunc(SceneUptr own);

	// 設定シーンへの生成関数
	SceneUptr MakeOptionFunc(SceneUptr own);

	// ダイアログ生成関数
	SceneUptr MakeDialogFunc(SceneUptr own);

#ifdef _DEBUG
	SceneUptr MakeTitleFunc(SceneUptr own);
#endif

	void UpdateOpend(float delta, Controller& controller) final;

	void DrawWindow(void) final;

	void Closed(void) final;

	bool IsLoaded(void) final;

	void Init(void);

	void DrawBackGround(void) final;

	void Back(void) final;

	void Loaded(Controller& controller);
	
	/// <summary>
	/// シャドウマップの作成
	/// </summary>
	/// <param name=""></param>
	void SetupShadowMap(void);


	/// <summary> 環境を描画する面の数 </summary>
/// <param name=""></param>
	void cubeDraw(void);

	// 選んだID
	SceneID choiceNextID_;

	// 枠
	SharedGraphicHandle frame_;

	// シャドウマップ用の深度テクスチャ
	// シャドウマップ用シェーダ
	SharedShaderHandle shadowPs_;
	int shadowMap_;
	LIGHT_MAT* lightMat_;
	LIGHT_MAT lightMat;
	int shadowBuff_;

	// 選択したステージ
	StageID staegID_;

	// 設定画面から復帰した時用のカーソルの位置
	Vector2 cursorPos_;
	// 経過時間
	float stepTime_;

	int skyDomeModel_;
	// キューブマップテクスチャ
	int dynamicCubeTex_;
	int cubeTex_;

};

