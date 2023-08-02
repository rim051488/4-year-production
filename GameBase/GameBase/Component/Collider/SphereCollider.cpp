#include "SphereCollider.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/Debug.h"

SphereCollider::SphereCollider() :
	radius_{0.0f}
{
}

const Vector3 SphereCollider::GetPos(void) const
{
	if (!transform_.IsActive())
	{
		return offset_;
	}


	auto offsetVec = transform_->GetRotation() * offset_;
	Vector3 pos = transform_->GetPos();
	pos += offsetVec;
	DebugDrawLine(transform_->GetPos(), pos, 0xffffff);
	return pos;
}

#ifdef _DEBUG

void SphereCollider::DrawDebug(void)
{
	if (IsActive())
	{
		auto pos{ GetPos() };
		DebugDrawSphere(pos, radius_, 0xffffff);
	}
}

#endif

void SphereCollider::Check(Collider& col, ObjectManager& objectManager)
{
	if (col.IsActive() && IsActive())
	{
		if (col.Check(*this, objectManager))
		{
			// ìñÇΩÇ¡ÇΩéûÇÃèàóù
			Hit(col, objectManager);
			col.Hit(*this, objectManager);
		}
	}
}

bool SphereCollider::Check(CapsuleCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(col, *this);
}

bool SphereCollider::Check(SphereCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

bool SphereCollider::Check(MeshCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

bool SphereCollider::Check(CharactorCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

bool SphereCollider::Check(TerrainCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col,objectManager);
}


