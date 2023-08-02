#pragma once
#include <map>
#include <array>
#include <string>
#include "InputID.h"
#include "../Vector2.h"
#include "../../SceneManager.h"

// トリガー情報
enum class Trg
{
	Now,				// 現在
	Old,				// 一つ前
	Max
};

// 入力装置の種類
enum class CntType
{
	Key,				// キーボード
	Pad,				// パッド
	Max
};

using InputData = std::array<std::pair<bool, bool>, static_cast<size_t>(InputID::Max) + 1>;

class Controller
{
public:
	bool operator()(float delta)
	{
		if (this != nullptr)
		{
			Update(delta);
		}
		return this != nullptr;
	}
	Controller();
	virtual ~Controller();

	/// <summary> 初期化 </summary>
	/// <returns> 成功時true失敗時false </returns>
	virtual bool Init(void) = 0;

	/// <summary> 更新 </summary>
	/// <param name="delta"> デルタタイム </param>
	virtual void Update(float delta) = 0;

	/// <summary> コントローラーの種類の取得 </summary>
	/// <returns> コントローラーの種類 </returns>
	virtual CntType GetCntType(void) = 0;

	/// <summary> 今左クリックしているか </summary>
	/// <param name="id">キーの種類</param>
	/// <returns>成功時にtrue失敗時にfalse</returns>
	//bool MousePress(InputID id);

	/// <summary> 今押しているか </summary>
	/// <param name="id"> キーの種類 </param>
	/// <returns> 成功時true失敗時false </returns>
	bool Press(InputID id);

	/// <summary> 押した瞬間 </summary>
	/// <param name="id"> キーの種類 </param>
	/// <returns> 成功時true失敗時false </returns>
	bool Pressed(InputID id);

	/// <summary> 離した瞬間 </summary>
	/// <param name="id"> キーの種類 </param>
	/// <returns> 成功時true失敗時false </returns>
	bool Released(InputID id);

	/// <summary> 今押されていないとき </summary>
	/// <param name="id"> キーの種類 </param>
	/// <returns> 成功時true失敗時false </returns>
	bool NotPress(InputID id);

	/// <summary> 今何も押されていないか </summary>
	/// <returns> 成功時true失敗時false </returns>
	bool IsNeutral();

	/// <summary>
	/// カーソル位置をセット(デフォルト引数は中心)
	/// </summary>
	/// <param name="pos"> カーソルの位置 </param>
	virtual void SetCursorPos(const Vector2& pos = lpSceneMng.screenSize_<float> / 2.0f) = 0;

	/// <summary>
	/// カーソル位置の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> カーソルの座標 </returns>
	const Vector2& GetCursorPos(void) const&
	{
		return cursorPos_;
	}


	/// <summary>
	/// カーソルのスピードをセット
	/// </summary>
	/// <param name="speed"> スピード </param>
	void SetCursorSpeed(float speed)
	{
		cursorSpeed_ = speed;
	}


	/// <summary>
	/// 左(移動)用
	/// </summary>
	/// <param name=""></param>
	/// <returns> 入力された方向 </returns>
	const Vector2& GetLeftInput(void) const&
	{
		return leftInput_;
	}

	/// <summary>
	/// 視点移動に使う座標を取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 中心からの移動した画面上の座標 </returns>
	const Vector2& GetRightInput(void) const&
	{
		return rightInput_;
	}

	/// <summary>
	/// 決定ボタンが押されているか
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressDecision(void) const;

	/// <summary>
	/// 決定ボタンが押された瞬間か
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressedDecision(void) const;

	/// <summary>
	/// キャンセルボタンが押されたか
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressCancel(void) const;

	/// <summary>
	/// キャンセルボタンが押された瞬間か
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool PressdCancel(void) const;
protected:
	/// <summary> 入力情報 </summary>
	//CntData cntData_;
	InputData data_;

	// 決定下かの情報の情報
	std::pair<bool, bool> decisionData_;

	// キャンセル押したかのデータ
	std::pair<bool, bool> cancelData_;

	// カーソルの座標
	Vector2 cursorPos_;

	// カーソルのスピード(ui時に使用)
	float cursorSpeed_;

	// 移動用の入力された方向
	Vector2 leftInput_;

	// 振り向き用入力座標
	Vector2 rightInput_;

};

