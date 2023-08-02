#include <DxLib.h>
#include <array>
#include "ShrinkBuff.h"

ShrinkBuff::ShrinkBuff()
{
}

ShrinkBuff::~ShrinkBuff()
{
}

void ShrinkBuff::Shrink(int berforeScr, Vector2 pos, float red)
{
	int width, height;
	GetDrawScreenSize(&width, &height);
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
	verts[1].pos.x = pos.x + width / red;
	verts[1].pos.y = pos.y;
	verts[1].u = 1.0f;
	verts[1].v = 0.0f;
	// 左下
	verts[2].pos.x = pos.x;
	verts[2].pos.y = pos.y + height / red;
	verts[2].u = 0.0f;
	verts[2].v = 1.0f;
	// 右下
	verts[3].pos.x = pos.x + width / red;
	verts[3].pos.y = pos.y + height / red;
	verts[3].u = 1.0f;
	verts[3].v = 1.0f;
	DrawPrimitive2D(verts.data(), verts.size(), DX_PRIMTYPE_TRIANGLESTRIP, berforeScr, true);

}
