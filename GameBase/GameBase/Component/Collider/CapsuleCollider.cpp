#include "CapsuleCollider.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/Debug.h"

CapsuleCollider::CapsuleCollider() :
	height_{0.0f}, radius_{0.0f}
{
}

const Vector3 CapsuleCollider::GetTop(void) const
{
	if (!transform_.IsActive())
	{
		return offset_;
	}

	// オフセット分ずらした座標を求める
	auto offsetVec = transform_->GetRotation() * offset_;
	Vector3 pos = transform_->Pos() + Vector3{offsetVec.x, offsetVec.y, offsetVec.z};

	// height_分上方向の座標を返す
	pos += (transform_.Get()->GetUp() * height_);
	return pos;
}

const Vector3 CapsuleCollider::GetBottom(void) const
{
	if (!transform_.IsActive())
	{
		return offset_;
	}

	// オフセット分ずらした座標を求める
	auto offsetVec = transform_->GetRotation() * offset_;
	Vector3 pos = transform_->Pos() + Vector3{ offsetVec.x, offsetVec.y, offsetVec.z };

	// height分下方向の座標を返す
	pos += (-transform_.Get()->GetUp() * height_);
	return pos;
}

#ifdef _DEBUG

void CapsuleCollider::DrawDebug(void)
{
	if (IsActive())
	{
		auto top{ GetTop() };
		auto btm{ GetBottom() };
		DebugDrawCapsule(top, btm, radius_, 0xffffff);
	}
}

#endif

void CapsuleCollider::Check(Collider& col, ObjectManager& objectManager)
{
	if (col.IsActive() && IsActive())
	{
		if (col.Check(*this, objectManager))
		{
			// 当たった時の処理
			Hit(col, objectManager);
			col.Hit(*this, objectManager);
		}
	}
}

bool CapsuleCollider::Check(CapsuleCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

bool CapsuleCollider::Check(SphereCollider& col, ObjectManager& objectManager)
{
	
	return Collider::HitCheck(*this,col);
}

bool CapsuleCollider::Check(MeshCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

bool CapsuleCollider::Check(CharactorCollider& col, ObjectManager& objectManager)
{
	// 普通に当たり判定

	return Collider::HitCheck(*this, col);
}

bool CapsuleCollider::Check(TerrainCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

