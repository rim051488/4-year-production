#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Collider/Collider.h"
#include "../Render/EffectRender.h"
#include "../Transform/Transform.h"
#include "../../Factory/FactoryID.h"
class PlayerSkillBehavior :
	public Behavior
{
public:
	PlayerSkillBehavior();

	// 攻撃時間をセット
	void SetAttackTime(float attackTime)
	{
		attackTime_ = attackTime;
	}

	// 攻撃判定が有効になるまでの時間
	void SetAttackStartTime(float startTime)
	{
		attackStartTime_ = startTime;
	}

	// 再生カウントをセット
	void SetPlayTime(const float time)
	{
		animTime_ = time;
	}

	ComponentID GetID(void) const override
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::PlayerSkillBehavior };
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

	// オブジェクトが有効になった時に呼ばれる処理
	void Begin(ObjectManager& objectManager) final;

	// オブジェクトが無効になった時に呼ばれる処理
	void End(ObjectManager& objectManager) final;

	// ヒット時の処理
	void OnHit(Collider& col, ObjectManager& objectManager);

	// 破棄時の処理
	void Destory(ObjectManager& objManager) final;

	// 攻撃が有効の時の処理
	void UpdateAttack(ObjectManager& objectManager, float delta);

	// 攻撃が有効ではないときの更新
	void UpdateNonAttack(ObjectManager& objectManager, float delta);

	void (PlayerSkillBehavior::* update_)(ObjectManager&, float);

	// 攻撃中の時間
	float attackTime_;

	// 攻撃開始までの時間
	float attackStartTime_;

	// 再生カウント
	float animTime_;

	// コライダー
	ComponentHandle<Collider> collider_;

	// 自身のトランスフォーム
	ComponentHandle<Transform> transform_;

	ObjectID effectID_;
};

