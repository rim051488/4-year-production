#include "ModelRender.h"
#include <DxLib.h>
#include "../../Scene/BaseScene.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/ResourceMng.h"
#include "../../SceneManager.h"
#include <unordered_map>

#include "../../Common/Debug.h"

// 頂点タイプに合わせたシェーダパスのテーブル
const std::unordered_map<int, std::string> vsShaderNameTbl{
	{DX_MV1_VERTEX_TYPE_1FRAME, "Mesh.vso"},
	{DX_MV1_VERTEX_TYPE_4FRAME,"Mesh4.vso"},
	{DX_MV1_VERTEX_TYPE_NMAP_1FRAME,"NormMesh.vso"}
};


ModelRender::ModelRender()
{
	draw_ = &ModelRender::DrawShader;
	drawDepth_ = &ModelRender::DrawDepthShader;
}

ModelRender::~ModelRender()
{
}

void ModelRender::Draw(int shadowMap, int buff, int cubeTex )
{
	if (!IsCameraView())
	{
		return;
	}
	(this->*draw_)(shadowMap, buff,cubeTex);
	
}

void ModelRender::SetUpDepthTex(int ps, int buff)
{
	if (!IsCameraView())
	{
		return;
	}
	(this->*drawDepth_)(ps, buff);
}

std::string ModelRender::Load(std::ifstream& file)
{
	int num{0};
	std::string str;
	file.read(reinterpret_cast<char*>(&num), sizeof(num));
	str.resize(num);
	file.read(str.data(), sizeof(str[0]) * num);
	return str;
}

void ModelRender::SetNonShader(void)
{
	draw_ = &ModelRender::DrawNonShader;
	drawDepth_ = &ModelRender::DrawDepthNonShader;
}

void ModelRender::Load(const std::filesystem::path& path)
{
	lpSceneMng.GetResourceMng().LoadModel(handle_, path);
}


void ModelRender::Update(BaseScene& scene,ObjectManager& objectManager, float delt, Controller& controller)
{
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

void ModelRender::Begin(ObjectManager& objectManager)
{
	// ロード終了orオブジェクトが場に出る時にここにくる

	Render::Begin(objectManager);
	auto type = MV1GetTriangleListVertexType(*handle_, 0);
	if (id_ == ComponentID::StageBehavior)
	{
		lpSceneMng.GetResourceMng().LoadVS(vs_, "Resource/resource/Shader/Vertex/Meshwater.vso");
		lpSceneMng.GetResourceMng().LoadVS(shadowVs_, "Resource/resource/Shader/ShadowMap/" + vsShaderNameTbl.at(type));
	}
	else
	{
		if (vsShaderNameTbl.contains(type))
		{
			lpSceneMng.GetResourceMng().LoadVS(vs_, "Resource/resource/Shader/Vertex/" + vsShaderNameTbl.at(type));
			lpSceneMng.GetResourceMng().LoadVS(shadowVs_, "Resource/resource/Shader/ShadowMap/" + vsShaderNameTbl.at(type));
		}
		else
		{
			DebugLog("このタイプのシェーダは無いです");
		}
	}

	if (type == DX_MV1_VERTEX_TYPE_4FRAME)
	{
		normMap = LoadGraph(L"Resource/resource/Stage.fbm/ShedCorrugatedNormal.png");
	}
	else
	{
		normMap = -1;
	}
	if (id_ == ComponentID::StageBehavior)
	{
		lpSceneMng.GetResourceMng().LoadPS(ps_, "Resource/resource/Shader/Pixel/waterPS.pso");
	}
	else
	{
		if (type < 3)
		{
			lpSceneMng.GetResourceMng().LoadPS(ps_, "Resource/resource/Shader/Pixel/Tex.pso");
		}
		else
		{
			lpSceneMng.GetResourceMng().LoadPS(ps_, "Resource/resource/Shader/Pixel/NormTex.pso");
		}
	}
	lpSceneMng.GetResourceMng().LoadPS(shadowPs_, "Resource/resource/Shader/ShadowMap/ShadowMap.pso");

	int meshNum = MV1GetMeshNum(*handle_);
	for (int i = 0; i < meshNum; i++)
	{
		MV1SetMeshBackCulling(*handle_, i, DX_CULLING_NONE);
	}
}

void ModelRender::End(ObjectManager& objectManager)
{
	bb_.isCheck_ = false;
	bb_.ltSize_ = zeroVector3<float>;
	bb_.rbSize_ = zeroVector3<float>;
}

bool ModelRender::IsCameraView(void)
{
	if (bb_.isCheck_)
	{
		return bb_.IsHit(transform_->GetPos(), transform_->GetRotation());
	}

	return true;
}

void ModelRender::DrawShader(int shadowMap, int buff, int cubeTex)
{
	SetTextureAddressMode(DX_TEXADDRESS_CLAMP);

	// シェーダをセットして描画
	MV1SetUseOrigShader(true);
	SetUseVertexShader(*vs_);
	SetUsePixelShader(*ps_);
	UpdateShaderConstantBuffer(buff);
	SetShaderConstantBuffer(buff, DX_SHADERTYPE_VERTEX, 4);
	// シャドウマップをテクスチャとしてセット
	//SetUseTextureToShader(1, normMap);
	SetUseTextureToShader(3, shadowMap);
	// ここでセットしたシャドウマップのフィルタリングとアドレスUVの設定を行う
	//MV1SetTextureSampleFilterMode(*handle_, 1, DX_DRAWMODE_ANISOTROPIC);
	//MV1SetTextureAddressMode(*handle_, 1, DX_TEXADDRESS_MIRROR, DX_TEXADDRESS_MIRROR);

	MV1DrawModel(*handle_);
	MV1SetUseOrigShader(false);
	SetUseTextureToShader(1, -1);
	SetUseTextureToShader(3, -1);

	//MV1DrawModelDebug(*handle_, 0xaaffaa, TRUE, 5.0f, FALSE, FALSE);
}

void ModelRender::DrawNonShader(int shadowMap, int buff, int cubeTex)
{
	MV1DrawModel(*handle_);
}

void ModelRender::DrawDepthShader(int ps, int buff)
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

void ModelRender::DrawDepthNonShader(int ps, int buff)
{
}


ModelRender::CameraBB::CameraBB():
	isCheck_{false}
{
}

bool ModelRender::CameraBB::IsHit(const Vector3& pos, const Quaternion& rot) const&
{
	auto lt = pos + (rot * ltSize_);
	auto rb = pos + (rot * rbSize_);
	return !CheckCameraViewClip_Box(VGet(lt.x, lt.y, lt.z), VGet(rb.x, rb.y, rb.z));
}
