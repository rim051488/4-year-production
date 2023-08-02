#include "EffectBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../Info/ObjectInfo.h"
#include "../../Factory/Factory.h"

EffectBehavior::EffectBehavior() :
	factoryID_{FactoryID::FireExEffect}, type_{EffectBehaviorType::Non}, update_{ &EffectBehavior::UpdateNonSync}
{
}

void EffectBehavior::SetType(EffectBehaviorType type)
{
	if (type == type_)
	{
		return;
	}

	type_ = type;
	switch (type_)
	{
	case EffectBehaviorType::SyncPos:
		update_ = &EffectBehavior::UpdateSyncPos;
		break;
	case EffectBehaviorType::SyncRot:
		update_ = &EffectBehavior::UpdateSyncRot;
		break;
	case EffectBehaviorType::SyncAll:
		update_ = &EffectBehavior::UpdateSyncAll;
		break;
	case EffectBehaviorType::Non:
		update_ = &EffectBehavior::UpdateNonSync;
		break;
	default:
		break;
	}

}

void EffectBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	(this->*update_)(scene, objectManager, delta, controller);
}

void EffectBehavior::Begin(ObjectManager& objectManager)
{
	effectRender_ = objectManager.GetComponent<EffectRender>(ownerId_);
	effectRender_->Play();

	if (objectManager.IsActive(syncID_))
	{
		syncTransform_ = objectManager.GetComponent<Transform>(syncID_);
	}

	transform_ = objectManager.GetComponent<Transform>(ownerId_);
}

void EffectBehavior::End(ObjectManager& objectManager)
{
	// ƒoƒO–h~‚Å‰Šú‰»‚µ‚Æ‚­
	SetType(EffectBehaviorType::Non);
	syncID_ = ObjectID{};
	syncTransform_ = ComponentHandle<Transform>();
}

void EffectBehavior::Destory(ObjectManager& objManager)
{
	objManager.GetFactory(factoryID_).Destroy(ownerId_);
}

void EffectBehavior::UpdateNonSync(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (effectRender_->IsEnd())
	{
		// Ä¶I—¹‚É©•ª©g‚ğ”jŠü‚·‚é
		objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();
	}
}

void EffectBehavior::UpdateSyncPos(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (syncTransform_.IsActive())
	{
		auto diff = syncTransform_->GetRotation()* offset_;
		transform_->Pos() = syncTransform_->GetPos() + diff;
	}
	UpdateNonSync(scene, objectManager, delta, controller);
}

void EffectBehavior::UpdateSyncRot(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (syncTransform_.IsActive())
	{
		transform_->SetRotation(syncTransform_->GetRotation());
	}
	UpdateNonSync(scene, objectManager, delta, controller);
}

void EffectBehavior::UpdateSyncAll(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (syncTransform_.IsActive())
	{
		transform_->SetRotation(syncTransform_->GetRotation());
		auto diff = syncTransform_->GetRotation() * offset_;
		transform_->Pos() = syncTransform_->GetPos() + diff;
	}
	UpdateNonSync(scene, objectManager, delta, controller);
}
