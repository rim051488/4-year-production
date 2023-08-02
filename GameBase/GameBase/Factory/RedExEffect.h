#pragma once
#include "Factory.h"
class RedExEffect :
    public Factory
{
public:
	RedExEffect(ObjectManager& objectManager);

private:

	/// <summary>
	/// 生成処理
	/// </summary>
	/// <param name="id"> 生成するを指示したオブジェクトのID </param>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <returns></returns>
	ObjectID Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot = { 0.0f,0.0f,0.0f }) final;

	/// <summary>
	/// 破棄処理
	/// </summary>
	/// <param name="id"></param>
	void Destroy(ObjectID id) final;

	/// <summary>
	/// IDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> FactoryID </returns>
	const FactoryID GetID(void) const final
	{
		return FactoryID::RedExEffect;
	}
};

