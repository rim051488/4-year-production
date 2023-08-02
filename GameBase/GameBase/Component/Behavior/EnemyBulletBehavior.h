#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Transform/Transform.h"
#include "../Collider/SphereCollider.h"


class EnemyBulletBehavior :
	public Behavior
{
public:

	ComponentID GetID(void) const override
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::EnemyBulletBehavior };
private:
	void Begin(ObjectManager& objectManager) final;

	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

	/// <summary>
	/// ヒット時の処理
	/// </summary>
	/// <param name="col"> 相手のコライダー </param>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	void OnHit(Collider& col, ObjectManager& objectManager);

	void Destory(ObjectManager& objManager) final;

	// 自身のトランスフォーム
	ComponentHandle<Transform> transform_;

	// 当たり判定のクラス
	ComponentHandle<SphereCollider> collider_;

	// 開始位置
	Vector3 startPos_;
};

