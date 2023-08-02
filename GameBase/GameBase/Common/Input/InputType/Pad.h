#pragma once
#include <DxLib.h>
#include "../Controller.h"

constexpr int motionRange = 30000;                                  // スティックの反応可動域

// ゲームパッド用
class Pad :
	public Controller
{
public:
	Pad(int padType);
	~Pad();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool Init(void) override;

	/// <summary>
	/// アップデート
	/// </summary>
	/// <param name="delta"> デルタ </param>
	void Update(float delta) override;

	/// <summary>
	/// コントローラーのタイプの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> コントローラーのタイプ </returns>
	CntType GetCntType(void) override { return CntType::Pad; };

	/// <summary>
	/// ゲームパッドの情報を取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const DINPUT_JOYSTATE& GetPadState(void) const&
	{
		return state_;
	}

	const int GetPadType(void) const;
private:

	/// <summary>
	/// カーソルの座標をセットする
	/// </summary>
	/// <param name="pos"></param>
	void SetCursorPos(const Vector2& pos = lpSceneMng.screenSize_<float> / 2.0f) final;

	/// <summary>
	/// プレイステーション系のパッドの右スティックの更新
	/// </summary>
	/// <param name=""></param>
	void UpdatePsPad(float delta);

	/// <summary>
	/// xbox系のパッドの右スティックの更新
	/// </summary>
	/// <param name=""></param>
	void UpdateXboxPad(float delta);

	// ゲームパッドの情報
	DINPUT_JOYSTATE state_;

	// 現在のゲームパッドの種類
	int nowPadType_;

	// 更新処理
	void(Pad::* update_)(float);
};

