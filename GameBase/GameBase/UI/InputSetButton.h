#pragma once
#include "ButtonBase.h"
#include "../Common/Input/InputID.h"

// 設定画面のキーやPADの設定用ボタン
class InputSetButton :
	public ButtonBase
{
public:
	InputSetButton(InputID id, const Vector2& pos);
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
	/// カーソルが当たってる時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawHitCursor(void);

	/// <summary>
	/// カーソルが当たってないときの描画
	/// </summary>
	/// <param name=""></param>
	void DrawNonHitCursor(void);

	/// <summary>
	/// xbox系のコントローラー使用時描画
	/// </summary>
	/// <param name=""></param>
	void DrawNowXboxCode(void);

	/// <summary>
	/// キーボード使用時の描画
	/// </summary>
	/// <param name=""></param>
	void DrawNowKeyCode(void);

	/// <summary>
	/// ps系のコントローラー使用時描画
	/// </summary>
	/// <param name=""></param>
	void DrawNowPsCode(void);

	// 描画処理
	void (InputSetButton::* draw_)(void);

	// 現在の設定されたコードの描画
	void (InputSetButton::* drawNowCode_)(void);

	// InputID
	InputID id_;

	// inputIDの表示用グラフィック
	SharedDivGraphicHandle inputIDInfo_;

	// 現在の入力情報用グラフィック
	SharedDivGraphicHandle inputInfo_;

	// ボタンのグラフィック
	SharedGraphicHandle button_;

	// クリック時の枠
	SharedGraphicHandle frame_;

	// 現在のコード
	int nowCode_;
};

