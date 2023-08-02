#include <DxLib.h>
#include "../../Common/Math.h"
#include "../../Object/ObjectManager.h"
#include "CapsuleRender.h"

CapsuleRender::CapsuleRender() :
	color_{0xffffff}, height_{0.0f}, radius_{0.0f}
{
}

CapsuleRender::~CapsuleRender()
{
}

void CapsuleRender::Draw(int shadowMap, int buff, int cubeTex)
{
	DrawCapsule3D(VGet(top_.x, top_.y, top_.z), VGet(bottom_.x, bottom_.y, bottom_.z), radius_, 64, color_, color_, true);
}

void CapsuleRender::SetUpDepthTex(int ps, int buff)
{
}

void CapsuleRender::Load(const std::filesystem::path& path)
{
}

void CapsuleRender::Update(BaseScene& scene,ObjectManager& objectManager, float delta, Controller& controller)
{
	if (transform_.IsActive())
	{
		const auto& pos = transform_.Get()->GetPos();
		auto upVec = transform_.Get()->GetUp();
		top_ = pos + (upVec * height_ / 2.0f);
		bottom_ = pos + (-upVec * height_ / 2.0f);
	}
}
