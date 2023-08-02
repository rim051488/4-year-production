#pragma once
#include "WindowScene.h"
#include "../Common/Input/InputConfig/InputConfig.h"


// 設定画面等のシーン
class OptionScene :
    public WindowScene
{
public:
	OptionScene(SceneUptr owner);

	/// <summary>
	/// InputSetで使うInputをセットする
	/// </summary>
	/// <param name="id"></param>
	void SetInputID(const InputID id)
	{
		setID_ = id;
	}

	/// <summary>
	/// 指定のInputIDを指定のコードを使うようにセットする(InputSetSceneから使用される)
	/// </summary>
	/// <param name="id"> id </param>
	/// <param name="code"> 使用するコード </param>
	void SetInputCode(InputID id, int code);

	/// <summary>
	/// 現在のインプットコードを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	InputConfig::InputCode& GetInputCode(void)&;

	/// <summary>
	/// デフォルトに戻す
	/// </summary>
	/// <param name=""></param>
	void Default(void);

	/// <summary>
	/// 設定したものを使用する(保存する)
	/// </summary>
	/// <param name=""></param>
	void Save(void);

	/// <summary>
	/// ポストエフェクトのモノトーンの使用フラグをセット
	/// </summary>
	/// <param name="flag"></param>
	void SetPeMono(bool flag)
	{
		onPeMono_ = flag;
	}

	/// <summary>
	/// ポストエフェクトのモノトーンを使用するか?
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool IsPeMono(void) const
	{
		return onPeMono_;
	}

	/// <summary>
	/// ポストエフェクトのフォグのセット
	/// </summary>
	/// <param name="flag"></param>
	void SetPeFog(bool flag)
	{
		onPeFog_ = flag;
	}

	/// <summary>
	/// ポストエフェクトのフォグを使用するか？
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool IsPeFog(void) const
	{
		return onPeFog_;
	}

private:
	const SceneID GetID(void) const final
	{
		return SceneID::Option;
	}

	void UpdateOpend(float delta, Controller& controller) final;

	void DrawWindow(void) final;

	void Closed(void) final;

	/// <summary>
	/// 入力の設定シーン生成関数
	/// </summary>
	/// <param name="own"></param>
	/// <returns></returns>
	SceneUptr MakeInputSetFunc(SceneUptr own);

	// 枠
	SharedGraphicHandle frame_;

	// uiから指定されたものをInputSetに使う
	InputID setID_;

	// 設定した結果
	InputConfig::InputCode setResultcode_;

	// ポストエフェクトのモノトーンの設定
	bool onPeMono_;

	// ポストエフェクトのフォグの設定
	bool onPeFog_;

};

