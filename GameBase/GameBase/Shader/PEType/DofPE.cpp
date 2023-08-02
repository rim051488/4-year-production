#include <DxLib.h>
#include "DofPE.h"

#include "../../Common/Debug.h"

DofPE::DofPE(int postPS, Vector2 pos, Vector2 rate) :PEBase(postPS, pos, rate)
{
	DebugLog("DofPE生成開始");
	VS_[0] = LoadVertexShader(L"Resource/resource/Shader/PostEffect/Xblur.vso");
	VS_[1] = LoadVertexShader(L"Resource/resource/Shader/PostEffect/Yblur.vso");
	ps_ = LoadPixelShader(L"Resource/resource/Shader/PostEffect/aveblur.pso");

	// 重みの初期化
	DebugLog("重み初期化");
	weibuff_ = CreateShaderConstantBuffer(sizeof(WEIGHT_NUM) * 4);
	weight = static_cast<WEIGHT_NUM*>(GetBufferShaderConstantBuffer(weibuff_));
	DebugLog("重み計算");
	WeitghtsGaussian(NUM_WEIGHTS, 8.0f);
	weight[0] = weights_;

	// スクリーンサイズの初期化
	DebugLog("スクリーンサイズ初期化");
	scrbuff_ = CreateShaderConstantBuffer(sizeof(Vector2) * 4);
	screen = static_cast<Vector2*>(GetBufferShaderConstantBuffer(scrbuff_));
	screen_ = Vector2(0.0f, 0.0f);
	screen[0] = screen_;

	DebugLog("スクリーンサイズ生成");
	int x, y;
	GetDrawScreenSize(&x, &y);
	vertBlur_ = MakeScreen(x / 2, y / 2, false);
	sideBlur_ = MakeScreen(x / 2, y, false);
	scr1_ = MakeScreen(x, y);
	// ここを消すと洞窟に似合いそうなポストエフェクトができる
	scr2_ = MakeScreen(x, y);

	DebugLog("DofPE生成終了");
}

DofPE::~DofPE()
{
}

void DofPE::Update(float delta)
{
}

int DofPE::Draw(int beforeScr, int afterScr, int depth, int skyScr, int redScr)
{
	// 弱いブラー(平均ブラー)
	SetDrawScreen(scr1_);
	ClsDrawScreen();
	SetPostEffect(beforeScr, -1, -1, -1, ps_, pos_, rate_);
	//SetBackgroundColor(0, 0, 0);

	// 強いブラー
	// 横ブラーをかける
	DrawXblur(beforeScr);
	// 縦ブラーをかける
	DrawYblur();
	SetDrawScreen(scr2_);
	ClsDrawScreen();
	// できたブラーを元のスクリーンのサイズに戻す
	DefaultScreen(vertBlur_,afterScr, pos_, rate_);
	//SetBackgroundColor(128, 128, 128);

	SetDrawScreen(ownScr_);
	ClsDrawScreen();
	// 被写界深度のため徐々に弱いブラーをかけていって、そしてもっと遠いところは強いブラーをかける
	SetPostEffect(beforeScr, scr1_,scr2_,depth,postPS_,pos_,rate_);
	return ownScr_;

}

void DofPE::DrawXblur(int beforeScr)
{
	SetDrawScreen(sideBlur_);
	ClsDrawScreen();
	SetXYBlurEffect(pos_, beforeScr, Vector2(2, 1), VS_[0]);
}

void DofPE::DrawYblur()
{
	SetDrawScreen(vertBlur_);
	ClsDrawScreen();
	SetXYBlurEffect(pos_, sideBlur_, Vector2(1, 2), VS_[0]);
}
