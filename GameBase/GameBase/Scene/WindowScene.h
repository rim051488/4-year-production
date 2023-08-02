#pragma once
#include "BaseScene.h"

// メニューやウィンドウ風に開閉演出を入れるためのシーン
class WindowScene :
	public BaseScene
{
public:

	WindowScene(SceneUptr owner, float actTime, ScreenID wndScreenID,ScreenID screenID,SceneID sceneID);

	/// <summary>
	/// 開く
	/// </summary>
	/// <param name=""></param>
	void Open(void);

	/// <summary>
	/// 閉じる
	/// </summary>
	/// <param name=""></param>
	void Close(void);
protected:
	void Update(float delta, Controller& controller) final;

	/// <summary>
	/// 開く演出が終了時の更新処理
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	virtual void UpdateOpend(float delta, Controller& controller) = 0;

	/// <summary>
	/// ウィンドウ部分の描画
	/// </summary>
	/// <param name=""></param>
	virtual void DrawWindow(void) = 0;

	/// <summary>
	/// 閉じる演出が終了したら呼ばれる
	/// </summary>
	/// <param name=""></param>
	virtual void Closed(void);

	/// <summary>
	/// 背景に当たる部分の描画(デフォルトでは呼び出し主を描画)
	/// </summary>
	/// <param name=""></param>
	virtual void DrawBackGround(void);

	/// <summary>
	/// シーンを描画
	/// </summary>
	/// <param name=""></param>
	void DrawScene(void) final;

	/// <summary>
	/// 開くときの更新
	/// </summary>
	/// <param name="delta"> デルタ </param>
	/// <param name="controller"> コントローラー </param>
	void UpdateOpen(float delta, Controller& controller);

	/// <summary>
	/// 閉じる時の更新
	/// </summary>
	/// <param name="delta"> デルタ </param>
	/// <param name="controller"> コントローラー </param>
	void UpdateClose(float delta, Controller& controller);


	/// <summary>
	/// 開くときの描画
	/// </summary>
	/// <param name=""></param>
	void DrawOpen(void);

	/// <summary>
	/// 閉じる時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawClose(void);

	/// <summary>
	/// 開いた後の描画
	/// </summary>
	/// <param name=""></param>
	void DrawOpend(void);

	// 更新処理
	void (WindowScene::* update_)(float, Controller&);


	// 描画処理
	void (WindowScene::* draw_)(void);
	
	// 呼び出し主
	SceneUptr owner_;

	// 開閉に使うスクリーン
	SharedRenderTargetHandle window_;

	// 開閉演出にかかる時間
	const float actTime_;

	// 経過時間
	float step_;
};

