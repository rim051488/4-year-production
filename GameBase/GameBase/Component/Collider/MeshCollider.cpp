#include "MeshCollider.h"
#include <DxLib.h>
#include "../../Object/ObjectManager.h"

MeshCollider::MeshCollider()
{
}

MeshCollider::~MeshCollider()
{
}

void MeshCollider::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (render_.IsActive())
	{
		MV1RefreshReferenceMesh(*render_->GetHandle(),-1,true);
	}
}

void MeshCollider::Begin(ObjectManager& objectManager)
{
	render_ = objectManager.GetComponent<ModelRender>(ownerId_);
	MV1SetupCollInfo(*render_->GetHandle());

}

void MeshCollider::End(ObjectManager& objectManager)
{
	Collider::End(objectManager);
	if (render_.IsActive())
	{
		MV1TerminateCollInfo(*render_->GetHandle());
	}
}


void MeshCollider::Check(Collider& col, ObjectManager& objectManager)
{
	if (col.IsActive() && IsActive())
	{
		if (col.Check(*this, objectManager))
		{
			// “–‚½‚Á‚½Žž‚Ìˆ—
			Hit(col, objectManager);
			col.Hit(*this, objectManager);
		}
	}
}

bool MeshCollider::Check(CapsuleCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(col,*this);
}

bool MeshCollider::Check(SphereCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(col,*this);
}

bool MeshCollider::Check(MeshCollider& col, ObjectManager& objectManager)
{
	
	return Collider::HitCheck(*this, col);
}

bool MeshCollider::Check(CharactorCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this,col);
}

bool MeshCollider::Check(TerrainCollider& col, ObjectManager& objectManager)
{
	return Collider::HitCheck(*this, col);
}

const int MeshCollider::GetHandle(void) const
{
	if (render_.IsActive())
	{
		return  *render_->GetHandle();
	}
	return -1;
}

#ifdef _DEBUG
void MeshCollider::DrawDebug(void)
{
	if (IsActive())
	{
		MV1DrawModelDebug(*render_->GetHandle(), 0xaaffaa, TRUE, 5.0f, TRUE, FALSE);
	}
}

#endif
