#pragma once
#include "Factory.h"
#include <forward_list>
#include "../Common/SharedHandle.h"

class EnemyBulletBehavior;
class SphereCollider;
class ObjectInfo;
class ModelRender;
class Transform;

// 敵が発射する弾を生成するクラス
class EnemyBulletFactory :
	public Factory
{
public:
	EnemyBulletFactory(ObjectManager& objectManager);
	~EnemyBulletFactory();
	
	/// <summary>
	/// 生成処理
	/// </summary>
	/// <param name="id"> 生成するを指示したオブジェクトのID </param>
	/// <param name="pos"></param>
	/// <param name="rot"></param>
	/// <returns></returns>
	ObjectID Create(ObjectID id, const Vector3& pos, const Vector3& rot = { 0.0f,0.0f,0.0f }) final;
	
	/// <summary>
	/// 破棄処理
	/// </summary>
	/// <param name="id"></param>
	void Destroy(ObjectID id) final;
private:

	/// <summary>
	/// IDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const FactoryID GetID(void) const { return FactoryID::EnemyBullet; }
  
	// 動きに関するクラスのプール
	std::forward_list<std::unique_ptr<EnemyBulletBehavior>> behaviorPool_;

	// ハンドル
	SharedModelHandle model_;

};

