#pragma once
#include "BaseScene.h"
#include "StageID.h"

class ResultScene :
    public BaseScene
{
public:
	ResultScene(StageID stageID,ResultAttribute atr, SharedRenderTargetHandle& capture);

	/// <summary>
	/// 次のステージへ移行する処理をする
	/// </summary>
	/// <param name=""></param>
	void NextStage(void);
private:
	const SceneID GetID(void) const
	{
		return SceneID::Result;
	}
	SceneUptr MakeSelectFunc(SceneUptr own);
	SceneUptr MakeRetryFunc(SceneUptr own);
	SceneUptr MakeDialogFunc(SceneUptr own);

	// メニューUIの初期化をまとめる
	void Update(float delta, Controller& controller) final;
	void DrawScene(void) final;

	bool IsLoaded(void) final;

	void Back(void) final;

	// ゲーム終了時のキャプチャ
	SharedRenderTargetHandle capture_;

	// 背景の枠
	SharedGraphicHandle frame_;

	// ゲームシーンのステージID
	StageID stageID_;

	// ダイアログからの復帰時用
	Vector2 cursorPos_;
};
