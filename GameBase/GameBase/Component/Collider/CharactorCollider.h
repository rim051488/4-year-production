#pragma once
#include "Collider.h"

// 敵やプレイヤーの本体用当たり判定クラス(カプセルにいろいろ機能がついた)
class CharactorCollider :
	public Collider
{
public:
	CharactorCollider();
	
	/// <summary>
	/// 上側の座標
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const Vector3 GetTop(void) const;
	
	/// <summary>
	/// 下側の座標
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const Vector3 GetBottom(void) const;

	/// <summary>
	/// 半径の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const float GetRadius(void) const
	{
		return radius_;
	}

	/// <summary>
	/// 半径のセット
	/// </summary>
	/// <param name="radius"></param>
	void SetRadius(const float radius)
	{
		radius_ = radius;
	}


	/// <summary>
	/// 移動方向のセット
	/// </summary>
	/// <param name="moveDir"></param>
	void SetMoveDir(const Vector3& moveDir)
	{
		moveDir_ = moveDir;
	}

	/// <summary>
	/// 移動方向の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const Vector3& GetMoveDir(void) const&
	{
		return moveDir_;
	}

	/// <summary>
	/// 高さのセット
	/// </summary>
	/// <param name="height_"></param>
	void SetHeight(const float height)
	{
		height_ = height;
	}

	/// <summary>
	/// 高さの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const float GetHeight(void) const
	{
		return  height_;
	}


	/// <summary>
	/// スピードの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> スピード </returns>
	const float GetSpeed(void) const
	{
		return speed_;
	}

	/// <summary>
	/// スピードのセット
	/// </summary>
	/// <param name="speed"></param>
	void SetSpeed(const float speed)
	{
		speed_ = speed;
	}

	/// <summary>
	/// 地面に接地しているか?
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const bool IsGround(void) const
	{
		return isGround_;
	}

	/// <summary>
	/// 接地フラグをセットする
	/// </summary>
	/// <param name="flag"> フラグ </param>
	void SetGroundFlag(const bool flag)
	{
		isGround_ = flag;
	}

	/// <summary>
	/// 重力の値をセットする
	/// </summary>
	/// <param name="g"></param>
	void SetGravityPow(const float g)
	{
		gravityPow_ = g;
	}

	/// <summary>
	/// 重力の値を取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const float GetGravityPow(void) const
	{
		return std::abs(gravityPow_);
	}

#ifdef _DEBUG
	void DrawDebug(void) final;
#endif
	// コンポーネントID
	static constexpr ComponentID id_{ ComponentID::CapsuleCollider };
	ComponentID GetID(void) const override
	{
		return id_;
	}

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

	// 移動方向
	Vector3 moveDir_;

	// 移動スピード
	float speed_;

	// 設置しているか
	bool isGround_;

	// 重力(落下時の着地判定用)
	float gravityPow_;
};

