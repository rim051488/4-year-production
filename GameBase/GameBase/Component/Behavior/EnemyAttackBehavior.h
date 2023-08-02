#pragma once
#include "Behavior.h"
#include "../Render/ModelRender.h"

class EnemyAttackBehavior :
	public Behavior
{
public:
	ComponentID GetID(void) const override
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::EnemyAttackBehavior };
	void SetEnemyID(ObjectID& enemy)
	{
		enemyID_ = enemy;
	}
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;
	void End(ObjectManager& objectManager) final;
	void Hit(Collider& collider, ObjectManager& objectManager);
	void Destory(ObjectManager& objManager) final;
	ComponentHandle<ModelRender> enemyModel_;

	ComponentHandle<Transform> transform_;

	ComponentHandle<Transform> enemyTransform_;

	// 攻撃主の敵オブジェクトのID
	ObjectID enemyID_;

	// 同期するフレームのインデックス
	int frameIdx_;
};

