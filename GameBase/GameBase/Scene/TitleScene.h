#pragma once
#include "BaseScene.h"

class ObjectManager;


class TitleScene :
	public BaseScene
{
public:
	TitleScene();
	~TitleScene();
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


private:
	const SceneID GetID(void) const
	{
		return SceneID::Title;
	}
	SceneUptr MakeGameFunc(SceneUptr own);
	void Update(float delta, Controller& controller) final;
	void DrawScene(void) final;
	bool IsLoaded(void) final;
	
	/// <summary>
	/// 通常時の更新
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateNon(float delta, Controller& controller);

	/// <summary>
	/// ロゴを消すときの更新
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateLogoOff(float delta, Controller& controller);

	/// <summary>
	/// ロゴを消すときの描画
	/// </summary>
	/// <param name=""></param>
	void DrawLogoOff(void);

	/// <summary>
	/// 通常時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawNon(void);

	/// <summary>
	/// 背景オブジェクトを生成する
	/// </summary>
	/// <param name=""></param>
	void CreateBackGround(void);

	/// <summary>
	/// ロード完了時の処理
	/// </summary>
	/// <param name="controller"> コントローラー </param>
	void Loaded(Controller& controller);

	///// <summary>
	///// シャドウマップの作成
	///// </summary>
	///// <param name=""></param>
	//void SetupShadowMap(void);

	/// <summary> 環境を描画する面の数 </summary>
	/// <param name=""></param>
	void cubeDraw(void);

	///// <summary>
	///// 波シェーダーの実装
	///// </summary>
	///// <param name="model">モデル</param>
	///// <param name="vs">頂点シェーダー</param>
	///// <param name="ps">ピクセルシェーダー</param>
	///// <param name="dynamicCube">キューブ情報</param>
	///// <param name="cube"></param>
	//void DrawModelForMyShader(int model, int vs, int ps, int dynamicCube, int cube);

	// 更新処理
	void (TitleScene::* update_)(float, Controller&);

	// 描画処理
	void (TitleScene::* draw_)(void);

	// 経過時間
	float stepTime_;

	int skyDomeModel_;
	// キューブマップテクスチャ
	int dynamicCubeTex_;
	int cubeTex_;

};

