#pragma once
#include "../ComponentBase.h"

class Collider;
class ObjectManager;


// オブジェクトの動きを実行するクラス
class Behavior :
	public ComponentBase
{
public:

	virtual ~Behavior();

	ComponentID GetID(void) const override
	{
		return id_;
	}
	SetID(ComponentID::BehaviorBase, ComponentID::BehaviorBase)

	/// <summary>
	/// 破棄処理を別途行うための関数
	/// </summary>
	/// <param name="objManager"> オブジェクトマネージャー </param>
	virtual void Destory(ObjectManager& objManager){}
};

