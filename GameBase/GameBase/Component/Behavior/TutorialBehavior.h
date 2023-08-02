#pragma once
#include "StageBehavior.h"
#include "../../Common/Vector3.h"

#include "../../Common/SharedHandle.h"

// チュートリアルの種類
enum class TutorialType
{
	Move,				// 移動
	View,				// 視点
	Jump,				// ジャンプ
	Dash,				// ダッシュ
	Attack,				// 攻撃
	Enemy,				// 敵
	SkillMess,			// スキルの説明
	SkillChargeMess,	// スキルを溜めるためのスポーン
	SkillCharge,		// スキルを溜める
	Skill,				// スキルの使用
	End,				// 終了
	Max
};


class TutorialBehavior :
    public StageBehavior
{
public:
	TutorialBehavior();
	ComponentID GetID(void) const override
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::TutorialBehavior };

	const TutorialType GetNowType(void) { return nowType_; }
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;

	/// <summary>
	/// 各アクションの間の待ち時間用アップデート
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateWait(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 移動チュートリアル用アップデート
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateMove(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	///// <summary>
	///// 視点操作のチュートリアル用
	///// </summary>
	///// <param name="scene"></param>
	///// <param name="objectManager"></param>
	///// <param name="delta"></param>
	///// <param name="controller"></param>
	//void UpdateView(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	///// <summary>
	///// ジャンプチュートリアル用アップデート
	///// </summary>
	///// <param name="scene"></param>
	///// <param name="objectManager"></param>
	///// <param name="delta"></param>
	///// <param name="controller"></param>
	//void UpdateJump(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 攻撃チュートリアル用
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateAttack(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 敵を倒すチュートリアルの敵のスポーン部分用
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateEnemySpawn(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 敵を倒すチュートリアル用
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateEnemy(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// スキルを溜めるための敵をスポーンさせる用
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateEnemySpawnGage(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 敵をスポーンさせる
	/// </summary>
	/// <param name="objectManager"></param>
	/// <param name="pos"></param>
	void SpawnEnemy(ObjectManager& objectManager, const Vector3& pos);

	/// <summary>
	/// スキルを溜める用
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateGageSkill(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// スキルの説明
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateSkillMess(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// スキルチュートリアル用
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateSkill(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// チュートリアル終了時の更新
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateEnd(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	// 経過時間
	float step_;

	// 待つ時間
	float waitTime_;

	// それぞれのアクションの開始時のプレイヤーの座標
	Vector3 playerStartPos_;

	// 更新処理
	void (TutorialBehavior::* update_)(BaseScene&, ObjectManager&, float, Controller&);

	// 次の更新処理
	void (TutorialBehavior::* nextUpdate_)(BaseScene&, ObjectManager&, float, Controller&);

	// 現在のチュートリアルタイプ
	TutorialType nowType_;

	// 次のチュートリアルタイプ
	TutorialType nextType_;

	// 敵のID
	ObjectID enemyID_;

	// 矢印のID
	ObjectID arrow_;
};

