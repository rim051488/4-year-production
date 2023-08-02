#pragma once
#include "WindowScene.h"


class PauseScene :
	public WindowScene
{
public:
	PauseScene(SceneUptr owner);

	/// <summary>
	/// 戻る処理
	/// </summary>
	/// <param name=""></param>
	void BackGame(void);

	/// <summary>
	/// 終了(ダイアログ)
	/// </summary>
	/// <param name=""></param>
	void Dialog(void);

	/// <summary>
	/// オプション
	/// </summary>
	/// <param name=""></param>
	void Option(void);
private:
	const SceneID GetID(void) const
	{
		return SceneID::Pause;
	}

	/// <summary>
	/// ゲームシーンへ戻る処理
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakeGameFunc(SceneUptr own);

	/// <summary>
	/// ダイアログシーンを生成する関数
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakeDialogFunc(SceneUptr own);

	/// <summary>
	/// オプションシーンを生成する関数
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakeOptionFunc(SceneUptr own);


	void UpdateOpend(float delta, Controller& controller) final;

	void DrawWindow(void) final;

	void Closed(void) final;

	void Back(void) final;


	// 枠のグラフィックハンドル
	SharedGraphicHandle frame_;

	// 選んだID
	SceneID choiceNextID_;

	// 終了ダイアログもしくは設定画面からの復帰用
	Vector2 cursorPos_;
};

