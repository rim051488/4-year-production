#pragma once
#include "WindowScene.h"

// 終了時に確認するためのシーン
class DialogScene :
	public WindowScene
{
public:
	DialogScene(SceneUptr owner);

	/// <summary>
	/// OKの処理
	/// </summary>
	/// <param name=""></param>
	void Ok(void);

	/// <summary>
	/// Cancelの処理
	/// </summary>
	/// <param name=""></param>
	void Cancel(void);
private:
	const SceneID GetID(void) const
	{
		return SceneID::Dialog;
	}

	/// <summary>
	/// 開いた後の更新処理
	/// </summary>
	/// <param name="delta"> デルタ </param>
	/// <param name="controller"> コントローラー </param>
	void UpdateOpend(float delta, Controller& controller) final;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name=""></param>
	void DrawWindow(void) final;

	/// <summary>
	/// 閉じる演出が終わった時の処理
	/// </summary>
	/// <param name=""></param>
	void Closed(void) final;

	// 枠
	SharedGraphicHandle frame_;

	
};

