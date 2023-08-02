#include <array>
#include <DxLib.h>
#include "ColortoMono.h"

ColortoMono::ColortoMono(int postPS, Vector2 pos, Vector2 rate):PEBase(postPS, pos, rate)
{
	// 定数バッファの初期化
	timeBuff_ = CreateShaderConstantBuffer(sizeof(float) * 4);
	time_ = static_cast<float*>(GetBufferShaderConstantBuffer(timeBuff_));
	time = 0.0f;
}

ColortoMono::~ColortoMono()
{
}

void ColortoMono::Update(float delta)
{
	// 徐々にモノクロかをするように更新する
	if (time < 1.0f)
	{
		time += delta /2.0f;
	}
	else
	{
		// 1.0以上にならないように調整する
		time = 1.0f;
	}
}

int ColortoMono::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	SetDrawScreen(ownScr_);
	ClsDrawScreen();
	// 定数バッファに格納する
	time_[0] = time;
	// そのバッファをhlsl側に渡す
	SetBuffer(timeBuff_);
	SetPostEffect(beforeScr, -1, -1, -1, postPS_, pos_, rate_);
	return ownScr_;
}
