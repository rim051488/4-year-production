#include "StageRender.h"
#include <DxLib.h>
#include "../../Scene/BaseScene.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/ResourceMng.h"
#include "../../SceneManager.h"
#include <unordered_map>

#include "../../Common/Debug.h"

// 頂点タイプに合わせたシェーダパスのテーブル
const std::unordered_map<int, std::string> vsShaderNameTbl{
	{DX_MV1_VERTEX_TYPE_1FRAME, "water.vso"},
	{DX_MV1_VERTEX_TYPE_4FRAME,"water.vso"},
	{DX_MV1_VERTEX_TYPE_NMAP_1FRAME,"water.vso"}
};


StageRender::StageRender()
{
	draw_ = &StageRender::DrawShader;
	drawDepth_ = &StageRender::DrawDepthShader;
}

StageRender::~StageRender()
{
	DeleteShaderConstantBuffer(cbuff_);
	DeleteShaderConstantBuffer(cbuffFloat4_);
}

void StageRender::Draw(int shadowMap, int buff, int cubeTex)
{
	if (!IsCameraView())
	{
		return;
	}
	(this->*draw_)(shadowMap, buff, cubeTex);

}

void StageRender::SetUpDepthTex(int ps, int buff)
{
	if (!IsCameraView())
	{
		return;
	}
	(this->*drawDepth_)(ps, buff);
}

std::string StageRender::Load(std::ifstream& file)
{
	int num{ 0 };
	std::string str;
	file.read(reinterpret_cast<char*>(&num), sizeof(num));
	str.resize(num);
	file.read(str.data(), sizeof(str[0]) * num);
	return str;
}

void StageRender::SetNonShader(void)
{
	draw_ = &StageRender::DrawNonShader;
	drawDepth_ = &StageRender::DrawDepthNonShader;
}

void StageRender::Load(const std::filesystem::path& path)
{
	lpSceneMng.GetResourceMng().LoadModel(handle_, path);
	// 定数バッファ
	SetUseASyncLoadFlag(false);
	cbuff_ = CreateShaderConstantBuffer(sizeof(float) * 4);
	writer_ = static_cast<float*>(GetBufferShaderConstantBuffer(cbuff_));
	// タイム
	writer_[0] = 0.0f;
	// 重力
	writer_[1] = 9.8f;
	// π:pi
	writer_[2] = DX_PI_F;
	cbuffFloat4_ = CreateShaderConstantBuffer(sizeof(FLOAT4) * 4);
	writeFloat4_ = static_cast<FLOAT4*>(GetBufferShaderConstantBuffer(cbuffFloat4_));
	// カメラの位置
	writeFloat4_[0] = { 0.0f, 1000.0f, -500.0f,0.0f };
	SetUseASyncLoadFlag(true);
}


void StageRender::Update(BaseScene& scene, ObjectManager& objectManager, float delt, Controller& controller)
{
	writer_[0] += delt;

	if (transform_.IsActive())
	{
		const auto& pos = transform_.Get()->GetPos();
		const auto& scale = transform_.Get()->GetScale();
		MV1SetPosition(*handle_, VGet(pos.x, pos.y, pos.z));
		MV1SetScale(*handle_, VGet(scale.x, scale.y, scale.z));
		auto rot = transform_->GetRotation().ToEuler();
		rot += defaultRot_;
		MV1SetRotationXYZ(*handle_, { rot.x, rot.y, rot.z });


#ifdef _DEBUG
		if (bb_.isCheck_)
		{
			auto q = transform_->GetRotation();
			DebugDrawCube(pos + (q * bb_.ltSize_), pos + (q * bb_.rbSize_), 0xffffff);
		}
#endif
	}
}

void StageRender::Begin(ObjectManager& objectManager)
{
	// ロード終了orオブジェクトが場に出る時にここにくる

	Render::Begin(objectManager);
	auto type = MV1GetTriangleListVertexType(*handle_, 0);
	if (vsShaderNameTbl.contains(type))
	{
		lpSceneMng.GetResourceMng().LoadVS(vs_, "Resource/resource/Shader/Vertex/" + vsShaderNameTbl.at(type));
		lpSceneMng.GetResourceMng().LoadVS(shadowVs_, "Resource/resource/Shader/ShadowMap/" + vsShaderNameTbl.at(type));
	}
	else
	{
		DebugLog("このタイプのシェーダは無いです");
	}

	if (type == DX_MV1_VERTEX_TYPE_4FRAME)
	{
		normMap = LoadGraph(L"Resource/resource/Stage.fbm/ShedCorrugatedNormal.png");
	}
	else
	{
		normMap = -1;
	}
	if (type < 3)
	{
		lpSceneMng.GetResourceMng().LoadPS(ps_, "Resource/resource/Shader/Pixel/water.pso");
	}
	else
	{
		lpSceneMng.GetResourceMng().LoadPS(ps_, "Resource/resource/Shader/Pixel/water.pso");
	}

	int meshNum = MV1GetMeshNum(*handle_);
	for (int i = 0; i < meshNum; i++)
	{
		MV1SetMeshBackCulling(*handle_, i, DX_CULLING_NONE);
	}
}

void StageRender::End(ObjectManager& objectManager)
{
	bb_.isCheck_ = false;
	bb_.ltSize_ = zeroVector3<float>;
	bb_.rbSize_ = zeroVector3<float>;
}

bool StageRender::IsCameraView(void)
{
	if (bb_.isCheck_)
	{
		return bb_.IsHit(transform_->GetPos(), transform_->GetRotation());
	}

	return true;
}

void StageRender::DrawShader(int shadowMap, int buffint,int cubeTex)
{
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	//シェーダを有効にする
	MV1SetUseOrigShader(true);

	SetUseTextureToShader(0, cubeTex);

	SetUseVertexShader(*vs_);
	SetUsePixelShader(*ps_);
	MV1DrawModel(*handle_);
	// 定数のセット
	SetShaderConstantBuffer(cbuff_, DX_SHADERTYPE_VERTEX, 7);
	SetShaderConstantBuffer(cbuff_, DX_SHADERTYPE_PIXEL, 7);
	UpdateShaderConstantBuffer(cbuff_);
	SetShaderConstantBuffer(cbuffFloat4_, DX_SHADERTYPE_VERTEX, 8);
	SetShaderConstantBuffer(cbuffFloat4_, DX_SHADERTYPE_PIXEL, 8);
	UpdateShaderConstantBuffer(cbuffFloat4_);

	//シェーダを無効にする
	MV1SetUseOrigShader(false);
	
	SetUseTextureToShader(0, -1);
}

void StageRender::DrawNonShader(int shadowMap, int buff, int cubeTex)
{
	MV1DrawModel(*handle_);
}

void StageRender::DrawDepthShader(int ps, int buff)
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

void StageRender::DrawDepthNonShader(int ps, int buff)
{
}

StageRender::CameraBB::CameraBB() :
	isCheck_{ false }
{
}

bool StageRender::CameraBB::IsHit(const Vector3& pos, const Quaternion& rot) const&
{
	auto lt = pos + (rot * ltSize_);
	auto rb = pos + (rot * rbSize_);
	return !CheckCameraViewClip_Box(VGet(lt.x, lt.y, lt.z), VGet(rb.x, rb.y, rb.z));
}
