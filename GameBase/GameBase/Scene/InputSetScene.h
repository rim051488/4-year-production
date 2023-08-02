#pragma once
#include "BaseScene.h"

// キーやPADを設定で使うクラス(設定したいボタンを押してくださいみたいなやつ)
class InputSetScene :
    public BaseScene
{
public:
	InputSetScene(SceneUptr optionScene, InputID id);

private:
	const SceneID GetID(void) const final
	{
		return SceneID::InputSet;
	}

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
	/// 待機用アップデート
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateWait(float delta, Controller& controller);

	/// <summary>
	/// キーボードをセットするときのアップデート
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateKeyboardSet(float delta, Controller& controller);

	/// <summary>
	/// ゲームパッドをセットするときのアップデート
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdatePadSet(float delta, Controller& controller);

	/// <summary>
	/// セット終了して閉じる時
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateClose(float delta, Controller& controller);

	/// <summary>
	/// セット中の描画
	/// </summary>
	/// <param name=""></param>
	void DrawSet(void);

	/// <summary>
	/// セット終了して閉じるまでの描画
	/// </summary>
	/// <param name=""></param>
	void DrawClose(void);

	/// <summary>
	/// セットしたい値を登録する
	/// </summary>
	/// <param name="code"></param>
	void SetCode(int code);

	// 更新
	void (InputSetScene::* update_)(float, Controller& );

	// 描画
	void (InputSetScene::* draw_)(void);

	// オプションシーン
	SceneUptr optionScene_;

	// セットする対象のID
	InputID id_;

	// 経過時間
	float step_;

	// 入力を促すメッセージの画像
	SharedGraphicHandle mess_;

	// 入力されたことを知らせるメッセージの画像
	SharedGraphicHandle mess2_;

	// 入力できないものを選択した時のメッセージ画像
	SharedGraphicHandle mess3_;

	// 選択に成功したか？
	bool isSelectSuccess_;
};

