#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Transform/Transform.h"

class SkyBehavior :
    public Behavior
{
public:
	ComponentID GetID(void) const final
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::SkyBehavior };
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;

	// プレイヤーのトランスフォーム
	ComponentHandle<Transform> player_;

	// 自分自身のトランスフォーム
	ComponentHandle<Transform> transform_;
};

