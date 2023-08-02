#include <DxLib.h>
#include "Pad.h"
#include "../InputConfig/InputConfig.h"

#include "../../Debug.h"

Pad::Pad(int padType) :
	nowPadType_{padType}, update_{nullptr}
{
	Init();
}

Pad::~Pad()
{
}

bool Pad::Init(void)
{
	cursorSpeed_ *= 2.0f;
	if (nowPadType_ == DX_PADTYPE_DUAL_SHOCK_4 || nowPadType_ == DX_PADTYPE_DUAL_SENSE)
	{
		// PS系のコントローラーの時
		update_ = &Pad::UpdatePsPad;
	}
	else if (nowPadType_ == DX_PADTYPE_XBOX_360 || nowPadType_ == DX_PADTYPE_XBOX_ONE)
	{
		// xbox系のコントローラーの時
		update_ = &Pad::UpdateXboxPad;
	}
	else
	{
		// とりあえずxbox系と同じの入れとく
		update_ = &Pad::UpdateXboxPad;
	}

	// 0で初期化しとく
	std::fill(std::begin(state_.Buttons), std::end(state_.Buttons), 0);
	return true;
}

void Pad::Update(float delta)
{
	// パッドの情報を取得
	GetJoypadDirectInputState(DX_INPUT_PAD1, &state_);
	
	// ボタンの情報をセット
	for (auto& code : lpConfigMng.GetInputCode())
	{
		data_[static_cast<size_t>(code.first)].second = data_[static_cast<size_t>(code.first)].first;
		data_[static_cast<size_t>(code.first)].first = state_.Buttons[code.second];
	}

	// 左スティックの情報をセット
	leftInput_ = Vector2{ static_cast<float>(state_.Y), static_cast<float>(-state_.X) };
	if (leftInput_.SqMagnitude() <= Square(500.0f))
	{
		// 傾きが一定値以下の時ゼロにする
		leftInput_ = zeroVector2<float>;
	}
	else
	{
		// コントローラーの傾きを正規化してセットする
		leftInput_.Normalize();
		cursorPos_ += Vector2{ static_cast<float>(-leftInput_.y),static_cast<float>(leftInput_.x),} *cursorSpeed_ * delta;
	}

	// 右スティックと決定&キャンセルボタンの更新
	(this->*update_)(delta);
}

const int Pad::GetPadType(void) const
{
	return nowPadType_;
}

void Pad::SetCursorPos(const Vector2& pos)
{
	cursorPos_ = pos;
	rightInput_ = pos;
}

void Pad::UpdatePsPad(float delta)
{
	// 決定ボタンの更新
	decisionData_.second = decisionData_.first;
	decisionData_.first = state_.Buttons[1];

	// キャンセルボタンの更新
	cancelData_.second = cancelData_.first;
	cancelData_.first = state_.Buttons[9];

	// 右スティックの更新
	Vector2 move{ static_cast<float>(state_.Z),static_cast<float>(state_.Rz) };
	rightInput_ = zeroVector2<float>;
	if (move.SqMagnitude() <= Square(500.0f))
	{
		return;
	}
	move.Normalize();
	rightInput_ += move * cursorSpeed_ * delta;
}

void Pad::UpdateXboxPad(float delta)
{
	// 決定ボタンの更新
	decisionData_.second = decisionData_.first;
	decisionData_.first = state_.Buttons[0];
	
	// キャンセルボタンの更新
	cancelData_.second = cancelData_.first;
	cancelData_.first = state_.Buttons[7];

	// 右スティックの更新
	Vector2 move{ static_cast<float>(state_.Rx),static_cast<float>(state_.Ry) };
	if (move.SqMagnitude() <= Square(500.0f))
	{
		return;
	}
	move.Normalize();
	rightInput_ += move * cursorSpeed_ * delta;
}
