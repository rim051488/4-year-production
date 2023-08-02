#include "SkyBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../../Common/Debug.h"

void SkyBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	if (player_.IsActive())
	{
		// ƒvƒŒƒCƒ„[‚Æ“¯Šú‚·‚é
		transform_->Pos() = player_->GetPos();
	}
}

void SkyBehavior::Begin(ObjectManager& objectManager)
{
	player_ = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
}
