#include <DxLib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Keyboard.h"
#include "../InputConfig/InputConfig.h"
#include "../../../SceneManager.h"
#include "../../Debug.h"

Keyboard::Keyboard()
{
	keyData_.fill(0);
	Init();
}

Keyboard::~Keyboard()
{
}

bool Keyboard::Init(void)
{
	return true;
}

void Keyboard::Update(float delta)
{
	// キーデータをまとめて取得
	GetHitKeyStateAll(keyData_.data());

	// マウスの状態を取得
	auto mouseState = GetMouseInput();

	for (auto& code : lpConfigMng.GetInputCode())
	{
		data_[static_cast<size_t>(code.first)].second = data_[static_cast<size_t>(code.first)].first;
		if (code.second < 0)
		{
			// 0以下の時はマウスに関する情報をセットする
			data_[static_cast<size_t>(code.first)].first = std::abs(code.second) & mouseState;
		}
		else
		{
			// キーボードの情報をセットする
			data_[static_cast<size_t>(code.first)].first = keyData_[code.second];
		}
	}

	// キーボードの押した状態に合わせて方向を決める
	leftInput_ = zeroVector2<float>;
	leftInput_.x += (keyData_[KEY_INPUT_W] ? -1.0f : 0.0f);
	leftInput_.x += (keyData_[KEY_INPUT_S] ? 1.0f : 0.0f);
	leftInput_.y += (keyData_[KEY_INPUT_D] ? -1.0f : 0.0f);
	leftInput_.y += (keyData_[KEY_INPUT_A] ? 1.0f : 0.0f);

	// 正規化
	leftInput_.Normalize();

	// マウスの座標を取得
	int x = 0, y = 0;
	GetMousePoint(&x, &y);
	rightInput_ = cursorPos_ = Vector2{ static_cast<float>(x), static_cast<float>(y) };

	// 決定ボタンの更新
	decisionData_.second = decisionData_.first;
	decisionData_.first = mouseState & MOUSE_INPUT_LEFT;

	// キャンセルボタンの更新
	cancelData_.second = cancelData_.first;
	cancelData_.first = keyData_[KEY_INPUT_ESCAPE];
}

void Keyboard::SetCursorPos(const Vector2& pos)
{
	// 引数の座標をセットする
	SetMousePoint(static_cast<int>(pos.x), static_cast<int>(pos.y));
	cursorPos_ = pos;
}
