#include <DxLib.h>
#include "GaussPE.h"

GaussPE::GaussPE(int postPS, Vector2 pos, Vector2 rate):PEBase(postPS, pos, rate)
{
	VS_[0] = LoadVertexShader(L"Resource/resource/Shader/PostEffect/Xblur.vso");
	VS_[1] = LoadVertexShader(L"Resource/resource/Shader/PostEffect/Yblur.vso");
	// 重みの初期化
	weibuff_ = CreateShaderConstantBuffer(sizeof(WEIGHT_NUM) * 4);
	weight = static_cast<WEIGHT_NUM*>(GetBufferShaderConstantBuffer(weibuff_));
	WeitghtsGaussian(NUM_WEIGHTS, 8.0f);
	weight[0] = weights_;

	// スクリーンサイズの初期化
	scrbuff_ = CreateShaderConstantBuffer(sizeof(Vector2) * 4);
	screen = static_cast<Vector2*>(GetBufferShaderConstantBuffer(scrbuff_));
	screen_ = Vector2(0.0f, 0.0f);
	screen[0] = screen_;

	int x, y;
	GetDrawScreenSize(&x, &y);
	vertBlur_ = MakeScreen(x / 2, y / 2, false);
	sideBlur_ = MakeScreen(x / 2, y,false);
}

GaussPE::~GaussPE()
{
	for (int i = 0; i < VS_.size(); i++)
	{
		DeleteShader(VS_[i]);
	}
}

void GaussPE::Update(float delta)
{

}

int GaussPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	// 横ブラーをかける
	DrawXblur(beforeScr);
	// 縦ブラーをかける
	DrawYblur();

	SetDrawScreen(ownScr_);
	ClsDrawScreen();
	// 縦ブラーまでかけたものを元のスクリーンのサイズに戻して描画する
	DefaultScreen(vertBlur_,afterScr, pos_, rate_);
	return ownScr_;
}

void GaussPE::DrawXblur(int beforeScr)
{
	SetDrawScreen(sideBlur_);
	ClsDrawScreen();
	SetXYBlurEffect(pos_, beforeScr, Vector2(2, 1), VS_[0]);
}

void GaussPE::DrawYblur()
{
	SetDrawScreen(vertBlur_);
	ClsDrawScreen();
	SetXYBlurEffect(pos_, sideBlur_, Vector2(1, 2), VS_[0]);
}

