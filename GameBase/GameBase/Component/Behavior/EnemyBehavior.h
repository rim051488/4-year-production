#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Transform/Transform.h"
#include "../Collider/CharactorCollider.h"
#include "EnemyLaserSightBehavior.h"
#include "../Animator/Animator.h"

// 敵の動き用のクラス
class EnemyBehavior :
	public Behavior
{
public:

	// 射撃用データ
	struct ShotData
	{
		float shotInterval;			// 撃つまでのインターバル
		float burstInterval;		// バースト射撃の一発当たりのインターバル
		int burstNum;				// バースト射撃の発射数
	};

	EnemyBehavior();

	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

	ComponentID GetID(void) const override
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::EnemyBehavior };

	/// <summary>
	/// 移動スピードのセット
	/// </summary>
	/// <param name="speed"> スピード </param>
	void SetSpeed(const float speed)
	{
		speed_ = speed;
	}

	float GetEnemyPower(void){return Power;}

	/// <summary>
	/// 攻撃を受けた際の硬直時間をセット
	/// </summary>
	/// <param name="hitTime"> 硬直時間 </param>
	void SetHitTime(const float hitTime)
	{
		hitTime_ = hitTime;
	}

	/// <summary>
	/// 振り向き終了にかかる時間をセット
	/// </summary>
	/// <param name="turnTime"> 振り向き終わるまでの時間 </param>
	void SetTurnTime(const float turnTime)
	{
		turnTime_ = turnTime;
	}

	/// <summary>
	/// 停止する距離をセット
	/// </summary>
	/// <param name="distance"> 停止するまでの距離 </param>
	void SetStopDistance(const float distance)
	{
		stopDistance_ = distance;
	}

	/// <summary>
	/// ステージのオブジェクトのID
	/// </summary>
	/// <param name="id"> ステージオブジェクトのID </param>
	void SetStageID(const ObjectID& id)
	{
		stageID_ = id;
	}

	/// <summary>
	/// 敵の追跡開始までの距離をセット
	/// </summary>
	/// <param name="distance"> 追跡開始までの距離 </param>
	void SetSearchDistance(const float distance)
	{
		searchDistance_ = distance;
	}

	/// <summary>
	/// チュートリアルモードにする
	/// </summary>
	/// <param name=""></param>
	void Tutorial(void)
	{
		isTutorial_ = true;
	}

	/// <summary>
	/// レーザーサイトのオブジェクトIDをセットする
	/// </summary>
	/// <param name="id"></param>
	void SetLaserSightID(ObjectID& id)
	{
		laserSightID_ = id;
	}

	/// <summary>
	/// 射撃データをセットする
	/// </summary>
	/// <param name="data"></param>
	void SetShotData(const ShotData& data)
	{
		shotData_ = data;
	}

	/// <summary>
	/// レーザーサイトのオブジェクトIDを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectID& GetlaserSightID(void)&
	{
		return laserSightID_;
	}

private:
	// 攻撃力
	static constexpr float Power = 20.0f;
	// 耐久値
	static constexpr int HpMax = 3;

	/// <summary>
	/// 時間やカウンター用変数を初期化する
	/// </summary>
	/// <param name=""></param>
	void Init(void);

	void Begin(ObjectManager& objectManager) final;

	void End(ObjectManager& objectManager) final;

	/// <summary>
	/// 追跡前の探知状態の更新
	/// </summary>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="delta"> デルタタイム </param>
	void UpdateSearch(ObjectManager& objectManager, float delta);

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="delta"> デルタタイム </param>
	void Shot(ObjectManager& objectManager,float delta);
	
	/// <summary>
	/// 発射しないとき
	/// </summary>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	void NonShot(ObjectManager& objectManager, float delta);

	/// <summary>
	/// 追跡移動時の処理
	/// </summary>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="delta"> デルタタイム </param>
	void UpdateMove(ObjectManager& objectManager, float delta);

	/// <summary>
	/// 停止状態の処理
	/// </summary>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="delta"> デルタタイム </param>
	void UpdateStop(ObjectManager& objectManager, float delta);

	/// <summary>
	/// 攻撃を受けた状態の処理
	/// </summary>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	/// <param name="delta"> デルタタイム </param>
	void UpdateHit(ObjectManager& objectManager, float delta);

	///// <summary>
	///// 武器を入れ替える時の処理
	///// </summary>
	///// <param name="objectManager"></param>
	///// <param name="delta"></param>
	//void UpdateGunToSword(ObjectManager& objectManager, float delta);

	///// <summary>
	///// 近接攻撃時の処理
	///// </summary>
	///// <param name="objectManager"></param>
	///// <param name="delta"></param>
	//void UpdateAttackSword(ObjectManager& objectManager, float delta);

	///// <summary>
	///// 近接攻撃から武器を切り替える時
	///// </summary>
	///// <param name="objectManager"></param>
	///// <param name="delta"></param>
	//void UpdateSwordToGun(ObjectManager& objectManager, float delta);

	/// <summary>
	/// 回転の処理
	/// </summary>
	/// <param name="delta"> デルタタイム </param>
	void Rotation(float delta);

	/// <summary>
	/// 重力処理(ノックバック用)
	/// </summary>
	/// <param name="delta"> デルタタイム </param>
	//void Gravity(float delta);


	/// <summary>
	/// ヒット時の処理
	/// </summary>
	/// <param name="col"> 当たった相手のコライダー </param>
	/// <param name="objectManager"> オブジェクトマネージャー </param>
	void OnHit(Collider& col, ObjectManager& objectManager);

	/// <summary>
	/// ノックバック処理
	/// </summary>
	/// <param name="objectManager"></param>
	void KnockBack(ObjectManager& objectManager);


	// 自身のトランスフォーム
	ComponentHandle<Transform> transform_;

	// プレイヤーのトランスフォーム
	ComponentHandle<Transform> playerT_;

	// 自身のコライダー
	ComponentHandle<CharactorCollider> collider_;

	// レーザー
	ComponentHandle<EnemyLaserSightBehavior> laserSight_;

	//// アニメーター
	//ComponentHandle<Animator> animetor_;

	// レーザーサイトのID
	ObjectID laserSightID_;

	// 移動スピード
	float speed_;

	//// ジャンプのスピード
	//float jumpTime_;

	// 停止する距離
	float stopDistance_;

	// 探知開始する距離
	float searchDistance_;

	// 被弾カウント
	int damageCnt_;

	// 受けた攻撃のコンボ
	int hitCombo_;

	// 現在の状態に遷移してからの時間
	float stateTime_;

	// 回転方向(-1か1)
	float rotSing_;

	// 回転を行うか?
	bool isRot_;

	// 回転して振り向き終えるまでの時間
	float turnTime_;

	// 攻撃を受けた際の硬直時間
	float hitTime_;

	// 射撃に関するデータ
	ShotData shotData_;

	// 射撃時間
	float shotTime_;

	// バースト射撃回数
	int burstCnt_;

	// 更新処理
	void (EnemyBehavior::* update_)(ObjectManager&, float);

	// ステージオブジェクトのID
	ObjectID stageID_;

	// ノックバック方向
	Vector3 knockBackVec_;

	// チュートリアル中か?
	bool isTutorial_;

	// 射撃処理
	void (EnemyBehavior::* shot_)(ObjectManager&, float);

	// 近接攻撃のオブジェクトID
	ObjectID attackID_;
};

