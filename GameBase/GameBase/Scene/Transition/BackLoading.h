#pragma once
#include "Loading.h"

// 背景に遷移前のシーンを描画するクラス
class BackLoading :
    public Loading
{
public:
	BackLoading(SceneUptr befor, SceneUptr after, float transTime);
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

};

