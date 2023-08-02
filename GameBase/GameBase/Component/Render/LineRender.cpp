#include "LineRender.h"
#include <DxLib.h>
#include "../../Object/ObjectManager.h"

void LineRender::Load(const std::filesystem::path& path)
{
}

void LineRender::Draw(int shadowMap, int buff, int cubeTex)
{
	if (isDraw_)
	{
		auto start = transform_->GetPos();
		auto end = start + (transform_->GetForward() * length_);
		DrawLine3D(VGet(start.x, start.y, start.z), VGet(end.x, end.y, end.z), color_);
	}
}

void LineRender::SetUpDepthTex(int ps, int buff)
{
}
