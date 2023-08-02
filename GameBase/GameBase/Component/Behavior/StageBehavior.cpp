#include "StageBehavior.h"
#include "../../Object/ObjectManager.h"
#include "../../Component/Transform/Transform.h"
#include "../../Common/SoundPross.h"
#include "../../Scene/GameScene.h"

StageBehavior::StageBehavior(std::map<size_t, Vector2>&& posList):
	enemyPosList_{std::move(posList)}
{
}

StageBehavior::StageBehavior()
{
}

void StageBehavior::SubEnemy(ObjectID& id)
{
	enemyPosList_.erase(*id);
}

const std::map<size_t, Vector2>& StageBehavior::GetEnemyPosList(void) const&
{
	return enemyPosList_;
}

void StageBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	// 敵の座標のリストを更新
	UpdateEnemyPosList(objectManager);

	if (enemyPosList_.size() <= 0)
	{
		// 敵をすべて倒したのでシーンを変える
		scene.ChangeSceneID(SceneID::Result);
		static_cast<GameScene&>(scene).SetResult(ResultAttribute::Clear);
	}

	auto player = (objectManager.GetComponent<Transform>(objectManager.GetPlayerID()));
	if (!player.IsActive())
	{
		// ゲームオーバー
		static_cast<GameScene&>(scene).SetResult(ResultAttribute::GameOver);
		scene.ChangeSceneID(SceneID::Result);
		lpSooundPross.Release();
	}
}

void StageBehavior::UpdateEnemyPosList(ObjectManager& objectManager)
{
	for (auto& pair : enemyPosList_)
	{
		auto trans = objectManager.GetComponent<Transform>(ObjectID{ pair.first });
		if (trans.IsActive())
		{
			// 敵の座標を格納する
			pair.second.x = trans->GetPos().x;
			pair.second.y = trans->GetPos().z;
		}
	}
}
