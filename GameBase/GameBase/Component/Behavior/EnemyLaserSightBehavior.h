#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Transform/Transform.h"
#include "../Render/LineRender.h"

// 敵の攻撃時の視線のクラス
class EnemyLaserSightBehavior :
    public Behavior
{
public:
	
	/// <summary>
	/// 同期する敵のIDをセットする
	/// </summary>
	/// <param name="id"></param>
	void SetEnemyID(ObjectID& id)
	{
		enemyID_ = id;
	}

	/// <summary>
	/// レーザーサイトをオンにする
	/// </summary>
	/// <param name=""></param>
	void On(void) const
	{
		line_->SetDrawFlag(true);
	}

	/// <summary>
	/// レーザーサイトをオフにする
	/// </summary>
	/// <param name=""></param>
	void Off(void) const
	{
		line_->SetDrawFlag(false);
	}

	/// <summary>
	/// オフセットをセットする
	/// </summary>
	/// <param name="offset"></param>
	void SetOffset(const Vector3& offset)
	{
		offset_ = offset;
	}

	ComponentID GetID(void) const override
	{
		return id_;
	}

	static constexpr ComponentID id_{ ComponentID::EnemyLaserSightBehavior };
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;


	// 同期すべき敵のID
	ObjectID enemyID_;

	// 同期先からのオフセット
	Vector3 offset_;

	// 同期先の敵
	ComponentHandle<Transform> enemy_;

	// 自身のトランスフォーム
	ComponentHandle<Transform> transform_;

	// 線を描画するクラス
	ComponentHandle<LineRender> line_;
};

