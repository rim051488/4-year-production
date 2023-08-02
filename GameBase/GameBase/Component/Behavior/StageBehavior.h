#pragma once
#include "Behavior.h"
#include <map>
#include "../../Common/Vector2.h"

class StageBehavior :
	public Behavior
{
public:
	StageBehavior(std::map<size_t, Vector2>&& posList);
	StageBehavior();

	/// <summary>
	/// 敵の数のカウントを減らす
	/// </summary>
	/// <param name=""></param>
	void SubEnemy(ObjectID& id);

	/// <summary>
	/// 敵の座標のリストを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const std::map<size_t, Vector2>& GetEnemyPosList(void) const&;
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) override;

protected:

	/// <summary>
	/// 敵の座標リストを更新する
	/// </summary>
	/// <param name="objectManager"></param>
	void UpdateEnemyPosList(ObjectManager& objectManager);

	// 敵の座標のリスト
	std::map<size_t, Vector2> enemyPosList_;
};

