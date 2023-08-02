#pragma once
#include "ButtonBase.h"
#include <functional>

class BaseScene;

// クリックできるボタン
class Button :
    public ButtonBase
{
	using PushFunc = std::function<void(BaseScene&)>;
public:
	Button(const PushFunc& pushFunc, const std::filesystem::path& btnPath, const std::filesystem::path& hitCursorPath, const Vector2& pos);
	void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) final;
	void Draw(int mainScr) final;

private:
	void Push(BaseScene& scene, Controller& controller) final;
	void HitCursor(void) final;
	void NotHitCursor(void)final;

	UiID GetUIID(void) final
	{
		return UiID::Button;
	}
	void Loaded(BaseScene& scene) final;

	/// <summary>
	/// カーソルと当たっている時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawHitCursor(void);

	/// <summary>
	/// カーソルと当たってないときの描画
	/// </summary>
	/// <param name=""></param>
	void DrawNotHitCursor(void);

	// 描画
	void(Button::* draw_)(void);

	// ボタン
	SharedGraphicHandle btnHandle_;

	// カーソルと当たってる時
	SharedGraphicHandle hitCursorHandle_;

	// ボタンを押したときの処理
	PushFunc pushFunc_;
};

