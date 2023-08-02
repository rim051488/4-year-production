#include "NonLightingRender.h"
#include <DxLib.h>
#include "../../Scene/BaseScene.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/ResourceMng.h"
#include "../../SceneManager.h"
#include <unordered_map>


//// 頂点タイプに合わせたシェーダパスのテーブル
//const std::unordered_map<int, std::string> vsShaderNameTbl{
//	{DX_MV1_VERTEX_TYPE_1FRAME, "Mesh.vso"},
//	{DX_MV1_VERTEX_TYPE_4FRAME,"Mesh4.vso"}
//};

void NonLightingRender::Draw(int shadowMap, int buff, int cubeTex )
{
	SetUseLighting(false);
	MV1DrawModel(*handle_);
	SetUseLighting(true);
}

void NonLightingRender::SetUpDepthTex(int ps, int buff)
{
	MV1SetUseOrigShader(true);
	SetUsePixelShader(ps);
	SetUseVertexShader(*shadowVs_);
	UpdateShaderConstantBuffer(buff);
	SetShaderConstantBuffer(buff, DX_SHADERTYPE_PIXEL, 6);
	MV1DrawModel(*handle_);
	MV1SetUseOrigShader(false);
	SetShaderConstantBuffer(-1, DX_SHADERTYPE_PIXEL, 6);
}
