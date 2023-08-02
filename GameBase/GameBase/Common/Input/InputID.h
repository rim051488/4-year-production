#pragma once
#include <type_traits>

// キー情報
enum class InputID
{
	Jump,				// ジャンプ
	Attack,				// 攻撃
	Dash,				// ダッシュトリガー
	Skil,				// スキルの使用
	Max
};

static InputID begin(InputID) { return InputID::Jump; }
static InputID end(InputID) { return InputID::Max; }
static InputID operator++(InputID& state) { return (state = static_cast<InputID>(std::underlying_type<InputID>::type(state) + 1)); }
static InputID operator*(const InputID& state) { return state; }
