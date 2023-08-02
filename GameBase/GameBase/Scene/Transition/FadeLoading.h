#pragma once
#include "Loading.h"

// ロード画面とフェードを行うクラス
class FadeLoading :
    public Loading
{
public:
	FadeLoading(SceneUptr befor, SceneUptr after, float transTime);
	FadeLoading(SceneUptr after, float transTime);
private:

	/// <summary>
	/// アップデート処理
	/// </summary>
	/// <param name="delta"> デルタタイム </param>
	/// <param name="controller"> コントローラー </param>
	void Update(float delta, Controller& controller) final;

	/// <summary>
	/// シーンの描画
	/// </summary>
	/// <param name=""></param>
	void DrawScene(void) final;

	/// <summary>
	/// ロード前のシーンをフェードアウトさせる時の更新
	/// </summary>
	/// <param name="delta"> デルタ </param>
	/// <param name="controller"> コントローラー </param>
	void UpdateFadeOut(float delta, Controller& controller);

	/// <summary>
	/// ロード中の更新
	/// </summary>
	/// <param name="delta"> デルタ </param>
	/// <param name="controller"> コントローラー </param>
	void UpdateLoading(float delta, Controller& controller);

	/// <summary>
	/// 遷移後のシーンをフェードインさせる時の更新
	/// </summary>
	/// <param name="delta"> デルタ </param>
	/// <param name="controller"> コントローラー </param>
	void UpdateFadesIn(float delta, Controller& controller);

	/// <summary>
	/// フェードイン時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawFadeIn(void);

	/// <summary>
	/// ロード中の描画
	/// </summary>
	/// <param name=""></param>
	void DrawLoading(void);
	
	/// <summary>
	/// フェードアウト時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawFadeOut(void);

	// 更新
	void (FadeLoading::* update_)(float, Controller&);

	// 描画
	void (FadeLoading::* draw_)(void);
};

