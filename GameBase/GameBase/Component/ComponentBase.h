#pragma once
#include "ComponentID.h"
#include "../Object/ObjectID.h"

class BaseScene;
class ObjectManager;
class Controller;

#define SetID(a,b) static constexpr ComponentID id_{a}; static constexpr ComponentID baseID_{ b };

// ゲームオブジェクトのコンポーネントの基底クラス
class ComponentBase
{
public:
	virtual ~ComponentBase();

	/// <summary>
	/// オブジェクトが有効になった時に呼ばれる
	/// </summary>
	/// <param name="id">  </param>
	virtual void Begin(ObjectManager& objectManager);

	/// <summary>
	/// オブジェクトが無効になった時に呼ばれる
	/// </summary>
	/// <param name=""></param>
	virtual void End(ObjectManager& objectManager){}

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="scene"> 現在のシーン </param>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="delta"> デルタタイム </param>
	virtual void Update(BaseScene& scene,ObjectManager& objectManager, float delta, Controller& controller){}

	/// <summary>
	/// IDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual ComponentID GetID(void) const = 0;

	/// <summary>
	/// 持ち主のIDをセット
	/// </summary>
	/// <param name="id"> 持ち主のID </param>
	void SetOwnerID(ObjectID& id) { ownerId_ = id; }

	/// <summary>
	/// 持ち主のIDを取得
	/// </summary>
	/// <param name="">  </param>
	/// <returns> 持ち主のID </returns>
	ObjectID& GetOwnerID(void) { return ownerId_; }

	SetID(ComponentID::Base, ComponentID::Base)
protected:

	// 持ち主のID
	ObjectID ownerId_;
};


