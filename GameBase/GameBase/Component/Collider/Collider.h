#pragma once
#include "../ComponentBase.h"
#include <functional>
#include "../ComponentHandle.h"
#include "ColliderType.h"
#include "../Transform/Transform.h"

class CapsuleCollider;
class SphereCollider;
class MeshCollider;
class CharactorCollider;
class TerrainCollider;


// 当たり判定用クラスのもとになるクラス
class Collider :
	public ComponentBase
{
public:
	virtual ~Collider();
	void Begin(ObjectManager& objectManager) override;

	void End(ObjectManager& objectManager) override;
	
	// チェック用関数
	virtual void Check(Collider& col, ObjectManager& objectManager) = 0;
	virtual bool Check(CapsuleCollider& col, ObjectManager& objectManager) = 0;
	virtual bool Check(SphereCollider& col, ObjectManager& objectManager) = 0;
	virtual bool Check(MeshCollider& col, ObjectManager& objectManager) = 0;
	virtual bool Check(CharactorCollider& col, ObjectManager& objectManager) = 0;
	virtual bool Check(TerrainCollider& col, ObjectManager& objectManager) = 0;

	/// <summary>
	/// ヒット時に呼ぶクラス
	/// </summary>
	/// <param name="col"> 当たった相手のコライダー </param>
	void Hit(Collider& col, ObjectManager& objMng);

	/// <summary>
	/// オフセットのセット
	/// </summary>
	/// <param name="offset_"></param>
	void SetOffset(const Vector3& offset)
	{
		offset_ = offset;
	}

	/// <summary>
	/// オフセットの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const Vector3& GetOffet(void) const&
	{
		return offset_;
	}

	/// <summary>
	/// ヒット時に呼び出されるfunctionをセットする
	/// </summary>
	/// <param name="func"></param>
	void SetHitFunc(std::function<void(Collider&, ObjectManager&)>&& func)
	{
		hit_ = std::move(func);
	}

	/// <summary>
	/// 当たり判定が有効か
	/// </summary>
	/// <param name=""></param>
	/// <returns> 有効時true無効時false </returns>
	const bool IsActive(void) const
	{
		return isActive_;
	}

	/// <summary>
	/// 当たり判定を有効にするかをセットする
	/// </summary>
	/// <param name="flag"></param>
	void SetActiveFlag(const bool flag)
	{
		isActive_ = flag;
	}

	/// <summary>
	/// 押し出し処理を行うか？
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	bool IsBlock(void) const 
	{
		return isBlock_;
	}

	/// <summary>
	/// 押し出しに関するフラグをセットする
	/// </summary>
	/// <param name="flag"></param>
	void SetBlockFlag(const bool flag) 
	{
		isBlock_ = flag;
	}

 #ifdef _DEBUG
	virtual void DrawDebug(void) = 0;
#endif

	ComponentID GetID(void) const override
	{
		return id_;
	}
	SetID(ComponentID::Collider, ComponentID::Collider)

protected:

	// カプセルとの判定用関数
	bool HitCheck(CapsuleCollider& a, CapsuleCollider& b);
	bool HitCheck(CapsuleCollider& a, SphereCollider& b);
	bool HitCheck(CapsuleCollider& a, MeshCollider& b);
	bool HitCheck(CapsuleCollider& a, CharactorCollider& b);
	bool HitCheck(CapsuleCollider& a, TerrainCollider& b);

	// 球体との判定用関数
	bool HitCheck(SphereCollider& a, SphereCollider& b);
	bool HitCheck(SphereCollider& a, MeshCollider& b);
	bool HitCheck(SphereCollider& a, CharactorCollider& b);
	bool HitCheck(SphereCollider& a, TerrainCollider& b, ObjectManager& objectmanager);

	// メッシュとの判定用関数
	bool HitCheck(MeshCollider& a, MeshCollider& b);
	bool HitCheck(MeshCollider& a, CharactorCollider& b);
	bool HitCheck(MeshCollider& a, TerrainCollider& b);

	// キャラクターとの判定用関数
	bool HitCheck(CharactorCollider& a, CharactorCollider& b, ObjectManager& objectmanager);
	bool HitCheck(CharactorCollider& a, TerrainCollider& b, ObjectManager& objectmanager);

	// 地形との判定用関数
	bool HitCheck(TerrainCollider& a, TerrainCollider& b);

	// 持ち主からのオフセット
	Vector3 offset_;

	// 持ち主のトランスフォームクラス
	ComponentHandle<Transform> transform_;

	// 判定が有効か？
	bool isActive_{true};

	// 押し出しが有効か?
	bool isBlock_{ false };
private:

	// ヒット時に呼び出すfunction
	std::function<void(Collider&, ObjectManager&)> hit_;
};

