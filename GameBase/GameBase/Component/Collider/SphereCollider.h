#pragma once
#include "Collider.h"
class SphereCollider :
    public Collider
{
public:
	SphereCollider();

	/// <summary>
	/// ”¼Œa‚Ìæ“¾
	/// </summary>
	/// <param name=""></param>
	/// <returns> ”¼Œa </returns>
	const float GetRadius(void) const
	{
		return radius_;
	}

	/// <summary>
	/// ”¼Œa‚ÌƒZƒbƒg
	/// </summary>
	/// <param name="radius"> ƒZƒbƒg‚·‚é”¼Œa </param>
	void SetRadius(const float radius)
	{
		radius_ = radius;
	}

	/// <summary>
	/// À•W‚ğæ“¾(offsetl—¶Ï‚İ‚Ì)
	/// </summary>
	/// <param name=""></param>
	/// <returns> À•W </returns>
	const Vector3 GetPos(void) const;

	static constexpr ComponentID id_{ ComponentID::SphereCollider };
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


	// ”¼Œa
	float radius_;
};

