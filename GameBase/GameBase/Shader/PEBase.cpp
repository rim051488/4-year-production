#include <array>
#include <DxLib.h>
#include "PEBase.h"


PEBase::PEBase(int postPS, Vector2 pos, Vector2 rate)
{
	pos_ = pos;
	rate_ = rate;
	postPS_ = postPS;
	gaussPS_ = LoadPixelShader(L"Resource/resource/Shader/PostEffect/gauss.pso");
	shrink_ = std::make_shared<ShrinkBuff>();
	int x, y;
	GetDrawScreenSize(&x, &y);
	ownScr_ = MakeScreen(x, y, false);
}

PEBase::~PEBase()
{
	DeleteShader(postPS_);
	DeleteShader(gaussPS_);
}

void PEBase::SetBuffer(int buff)
{
	buff_ = buff;
}

void PEBase::SetPostEffect(int firstScr, int secondScr, int thirdScr, int depthScr, int postPS, Vector2 pos, Vector2 rate)
{
	int width, height;
	GetGraphSize(firstScr, &width, &height);
	std::array <VERTEX2DSHADER, 4> verts;
	for (auto& v : verts)
	{
		v.rhw = 1.0;
		v.dif = GetColorU8(0xff, 0xff, 0xff, 0xff); // ディフューズ
		v.spc = GetColorU8(255, 255, 255, 255);		// スペキュラ
		v.su = 0.0f;
		v.sv = 0.0f;
		v.pos.z = 0.0f;
	}
	// 左上
	verts[0].pos.x = pos.x;
	verts[0].pos.y = pos.y;
	verts[0].u = 0.0f;
	verts[0].v = 0.0f;
	// 右上
	verts[1].pos.x = pos.x + width * rate.x;
	verts[1].pos.y = pos.y;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;
	// 左下
	verts[2].pos.x = pos.x;
	verts[2].pos.y = pos.y + height * rate.y;
	verts[2].u = 0.0f;
	verts[2].v = 1.0f;
	// 右下
	verts[3].pos.x = pos.x + width * rate.x;
	verts[3].pos.y = pos.y + height * rate.y;
	verts[3].u = 1.0f;
	verts[3].v = 1.0f;
	SetUsePixelShader(postPS);
	UpdateShaderConstantBuffer(buff_);
	SetShaderConstantBuffer(buff_, DX_SHADERTYPE_PIXEL, 6);
	SetUseTextureToShader(0, firstScr);
	SetUseTextureToShader(1, depthScr);
	SetUseTextureToShader(2, secondScr);
	SetUseTextureToShader(3, thirdScr);
	DrawPrimitive2DToShader(verts.data(), verts.size(), static_cast<int>(DX_PRIMTYPE_TRIANGLESTRIP));
	MV1SetUseOrigShader(false);
	SetUseTextureToShader(0, -1);
	SetUseTextureToShader(1, -1);
	SetUseTextureToShader(2, -1);
	SetUseTextureToShader(3, -1);
}

void PEBase::DefaultScreen(int berforeScr, int afterScr, Vector2 pos, Vector2 rate)
{
	int width, height;
	GetGraphSize(afterScr, &width, &height);
	std::array <VERTEX2D, 4> verts;
	for (auto& v : verts)
	{
		v.rhw = 1.0;
		v.dif = GetColorU8(0xff, 0xff, 0xff, 0xff); // ディフューズ
		v.pos.z = 0.0f;
	}
	// 左上
	verts[0].pos.x = pos.x;
	verts[0].pos.y = pos.y;
	verts[0].u = 0.0f;
	verts[0].v = 0.0f;
	// 右上
	verts[1].pos.x = pos.x + width;
	verts[1].pos.y = pos.y;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;
	// 左下
	verts[2].pos.x = pos.x;
	verts[2].pos.y = pos.y + height;
	verts[2].u = 0.0f;
	verts[2].v = 1.0f;
	// 右下
	verts[3].pos.x = pos.x + width;
	verts[3].pos.y = pos.y + height;
	verts[3].u = 1.0f;
	verts[3].v = 1.0f;
	DrawPrimitive2D(verts.data(), verts.size(), DX_PRIMTYPE_TRIANGLESTRIP, berforeScr, true);

}

void PEBase::SetXYBlurEffect(Vector2 pos, int scr, Vector2 div, int vs)
{
	int width, height;
	GetGraphSize(scr, &width, &height);
	screen_ = Vector2(width, height);
	screen[0] = screen_;
	UpdateShaderConstantBuffer(scrbuff_);
	SetShaderConstantBuffer(scrbuff_, DX_SHADERTYPE_VERTEX, 5);
	std::array <VERTEX3DSHADER, 6> verts;
	UpdateShaderConstantBuffer(weibuff_);
	SetShaderConstantBuffer(weibuff_, DX_SHADERTYPE_PIXEL, 5);
	for (auto& v : verts)
	{
		v.norm = VGet(0.0f, 0.0f, -1.0f);
		v.dif = GetColorU8(0xff, 0xff, 0xff, 0xff); // ディフューズ
		v.spc = GetColorU8(255, 255, 255, 255);		// スペキュラ
		v.su = 0.0f;
		v.sv = 0.0f;
		v.pos.z = 0.0f;
	}
	// 左上
	verts[0].pos.x = pos.x;
	verts[0].pos.y = pos.y + height / div.y;
	verts[0].u = 0.0f;
	verts[0].v = 0.0f;
	// 右上
	verts[1].pos.x = pos.x + width / div.x;
	verts[1].pos.y = pos.y + height / div.y;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;
	// 左下
	verts[2].pos.x = pos.x;
	verts[2].pos.y = pos.y;
	verts[2].u = 0.0f;
	verts[2].v = 1.0f;
	// 右下
	verts[3].pos.x = pos.x + width / div.x;
	verts[3].pos.y = pos.y;
	verts[3].u = 1.0f;
	verts[3].v = 1.0f;
	// 左下
	verts[4] = verts[2];
	// 右上
	verts[5] = verts[1];

	SetUseVertexShader(vs);
	SetUsePixelShader(gaussPS_);

	SetUseTextureToShader(0, scr);
	DrawPolygon3DToShader(verts.data(), 2);
	MV1SetUseOrigShader(false);
	SetUseTextureToShader(0, -1);
}

void PEBase::WeitghtsGaussian(int Weights, float sigma)
{
	// 重みの合計を記録する変数を定義する
	float total = 0;

	// ループ変数のｘが基準テクセルからの距離
	for (int x = 0; x < Weights; x++)
	{
		weights_.weight[x] = expf(-5.0f * (float)(x * x) / sigma);
		total += 2.0f * weights_.weight[x];
	}

	// 重みの合計で除算することで、重みの合計を１にしている
	for (int i = 0; i < Weights; i++)
	{
		weights_.weight[i] /= total;
	}
}
