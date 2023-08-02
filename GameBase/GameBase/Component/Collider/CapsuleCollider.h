#pragma once
#include "Collider.h"

// カプセル型の当たり判定
class CapsuleCollider :
	public Collider
{
public:
	CapsuleCollider();

	/// <summary>
	/// 上側の座標の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 上側の座標 </returns>
	const Vector3 GetTop(void) const;

	/// <summary>
	/// 下側の座標の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 下側の座標 </returns>
	const Vector3 GetBottom(void) const;

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 半径 </returns>
	const float GetRadius(void) const
	{
		return radius_;
	}

	/// <summary>
	/// 半径のセット
	/// </summary>
	/// <param name="radius"> 半径 </param>
	void SetRadius(const float radius)
	{
		radius_ = radius;
	}

	/// <summary>
	/// 高さのセット
	/// </summary>
	/// <param name="height"> 高さ </param>
	void SetHeight(const float height)
	{
		height_ = height;
	}

	static constexpr ComponentID id_{ ComponentID::CapsuleCollider };
	ComponentID GetID(void) const override
	{
		return id_;
	}
#ifdef _DEBUG
	void DrawDebug(void) final;
#endif
private:
	void Check(Collider& col, ObjectManager& objectManager) final;
	bool Check(CapsuleCollider& col, ObjectManager& objectManager) final;
	bool Check(SphereCollider& col, ObjectManager& objectManager) final;
	bool Check(MeshCollider& col, ObjectManager& objectManager) final;
	bool Check(CharactorCollider& col, ObjectManager& objectManager) final;
	bool Check(TerrainCollider& col, ObjectManager& objectManager) final;

	// 高さ
	float height_;

	// 半径
	float radius_;
};

