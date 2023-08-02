#pragma once
#include "ButtonBase.h"
#include <functional>

class BaseScene;

class SwitchButton :
    public ButtonBase
{
public:
	using PushFunc = std::function<void(BaseScene&, bool)>;
	using UpdateFunc = std::function<bool(BaseScene&)>;
	SwitchButton(
		const PushFunc& pushFunc,
		const UpdateFunc& updateFunc,
		const std::filesystem::path& onPath,
		const std::filesystem::path& offPath,
		const std::filesystem::path& cursorPath,
		const Vector2& pos
	);
private:
	void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) final;
	void Draw(int mainScr) final;
	void Push(BaseScene& scene, Controller& controller) final;
	void HitCursor(void) final;
	void NotHitCursor(void)final;

	UiID GetUIID(void) final
	{
		return UiID::Button;
	}
	void Loaded(BaseScene& scene) final;

	/// <summary>
	/// ボタンがオンの時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawOnButton(void);

	/// <summary>
	/// ボタンがオフの時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawOffButton(void);

	/// <summary>
	/// カーソルが乗った時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawOnCursor(void);

	/// <summary>
	/// カーソルが乗ってないときの描画
	/// </summary>
	/// <param name=""></param>
	void DrawNotCursor(void);

	// ボタンの描画
	void(SwitchButton::* drawButton_)(void);

	// フレームの描画(カーソルが乗った時とかの)
	void(SwitchButton::* drawFrame_)(void);

	// オンフラグ
    bool isOn_;

	// ボタンがオンの時の描画
	SharedGraphicHandle onBtnHandle_;

	// ボタンがオフの時の描画
	SharedGraphicHandle offBtnHandle_;

	// カーソルと当たってる時
	SharedGraphicHandle hitCursorHandle_;

	// ボタンを押したときの処理
	PushFunc pushFunc_;

	// 更新処理
	UpdateFunc updateFunc_;
};

