#include "EnemyBehavior.h"
#include "../Transform/Transform.h"
#include "../../SceneManager.h"
#include "../../Object/ObjectManager.h"
#include "../Info/ObjectInfo.h"
#include "../Behavior/StageBehavior.h"
#include "PlayerBehavior.h"
#include "EnemyBulletBehavior.h"
#include "PlayerAttackBehavior.h"
#include "../../Common/SoundPross.h"
#include "../ComponentPool.h"
#include "EnemyAttackBehavior.h"


#include "../../Common/Debug.h"


// 回転が必要な範囲(角度)
constexpr float rotDeff{ Square(Deg2Rad(15.0f)) };

// ノックバック時の初速
constexpr float v0{ 9.0f };

// ノックバック用の重力加速度
constexpr float gravity{ 9.80f * 10.0f };

// コンストラクタ
EnemyBehavior::EnemyBehavior()
{
	// デフォルトのスピードとして120を入れとく
	speed_ = 150.0f;
	update_ = nullptr;
	stopDistance_ = 0.0f;
	searchDistance_ = 0.0f;
	// デフォルトの振り向き時間として1を入れとく
	turnTime_ = 1.0f;
	Init();
}

// 更新処理
void EnemyBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	stateTime_ += delta;

	// 重力処理
	//Gravity(delta);

	(this->*update_)(objectManager, delta);

}

// カウンタ変数や時間用変数の初期化
void EnemyBehavior::Init(void)
{
	shotTime_ = 0.0f;
	burstCnt_ = 0;
	damageCnt_ = 0;
	isRot_ = false;
	stateTime_ = 0.0f;
	hitTime_ = 0.0f ;
	rotSing_ = 0.0f ;
	hitCombo_ = 0;
	//jumpTime_ = 0.0f;
}


// 有効時の開始処理
void EnemyBehavior::Begin(ObjectManager& objectManager)
{
	// 変数を初期化
	Init();

	// トランスフォームを取得
	transform_ = objectManager.GetComponent<Transform>(ownerId_);

	// プレイヤーのトランスフォームを取得
	playerT_ = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());

	// コライダーを取得しヒット時の関数をセット
	collider_ = objectManager.GetComponent<CharactorCollider>(ownerId_);
	collider_->SetHitFunc(std::bind(&EnemyBehavior::OnHit, this,std::placeholders::_1, std::placeholders::_2));
	collider_->SetGroundFlag(true);

	// 更新用関数をセット
	update_ = &EnemyBehavior::UpdateMove;

	if (isTutorial_)
	{
		// チュートリアルの時は弾を撃たないようセット
		shot_ = &EnemyBehavior::NonShot;
	}
	else
	{
		shot_ = &EnemyBehavior::Shot;
	}
	
	// レーザーサイト取得
	laserSight_ = objectManager.GetComponent<EnemyLaserSightBehavior>(laserSightID_);

	//// アニメーター取得
	//animetor_ = objectManager.GetComponent<Animator>(ownerId_);

	//// アニメーションをセット
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::Wait), true);
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::WalkHaveGun), true);
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::GunToSword), false);
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::AttackSword), false);
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::AttackGun), true);
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::GetHit), false);
	//animetor_->AddAnimation(static_cast<int>(EnemyAnim::SwordToGun), false);
	//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::Wait));
}

// 終了処理
void EnemyBehavior::End(ObjectManager& objectManager)
{
	// コンポーネントが向こうになった時に呼ばれる
	auto stage = objectManager.GetComponent<StageBehavior>(stageID_);
	if (stage.IsActive())
	{
		// ステージが生きている時ステージの敵のカウントを減らさせる
		stage->SubEnemy(ownerId_);
	}
}

// 追跡前の状態の更新
void EnemyBehavior::UpdateSearch(ObjectManager& objectManager, float delta)
{
	if ((transform_->GetPos() - playerT_->GetPos()).SqMagnitude() <= Square(searchDistance_))
	{
		// 一定の距離になったら移動開始する
		stateTime_ = 0.0f;
		update_ = &EnemyBehavior::UpdateMove;
		//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::WalkHaveGun), 0.5f);
	}
}

// 射撃処理
void EnemyBehavior::Shot(ObjectManager& objectManager,float delta)
{
	shotTime_ += delta;
	if (shotTime_ >= 0.0f)
	{
		if (shotTime_ >= shotData_.burstInterval)
		{
			shotTime_ = 0.0f;
			burstCnt_++;
			auto shotPos = transform_->GetPos() + (transform_->GetRotation() * Vector3 { 10.0f, 190.0f, 120.0f });

			// 右上方向にずらして作成
			auto id = objectManager.CreateFromFactory(FactoryID::EnemyBullet, ownerId_, shotPos);
			objectManager.Begin(id);
			// マズルフラッシュエフェクト
			auto effect = objectManager.CreateFromFactory(FactoryID::MuzzleFlashEffect, ownerId_, shotPos, { 0.0f,transform_->GetRotation().ToEuler().y + Deg2Rad(180.0f), 0.0f});
			objectManager.GetComponent<Transform>(effect)->Scale() = Vector3{ 0.5f,0.5f,0.5f };
			objectManager.Begin(effect);

			if (burstCnt_ >= shotData_.burstNum)
			{
				// 規定の数発射したら0にする
				burstCnt_ = 0;
				shotTime_ = -shotData_.shotInterval;
			}
		}

		//// 座標の取得
		//auto bPos = VGet(shotPos.x, shotPos.y, shotPos.z);			// 弾の座標

		//// サウンドの座標を取得
		//Set3DPositionSoundMem(bPos, static_cast<int>(SOUNDNAME_SE::enemyAttack));

		//// サウンドが届く範囲を指定
		//Set3DRadiusSoundMem(256, static_cast<int>(SOUNDNAME_SE::enemyAttack));
	}
}

// 射撃しないときの処理
void EnemyBehavior::NonShot(ObjectManager& objectManager, float delta)
{
	// なにもしない
}

// 追跡移動状態の処理
void EnemyBehavior::UpdateMove(ObjectManager& objectManager, float delta)
{
	// 回転処理
	Rotation(delta);

	// 移動
	transform_->Pos() += transform_->GetForward() * (delta * speed_);
	collider_->SetSpeed((delta * speed_));
	collider_->SetMoveDir(transform_->GetForward());

	//if (!animetor_->GetAnimState()->IsBlend())
	//{
	if ((transform_->GetPos() - playerT_->GetPos()).SqMagnitude() <= Square(stopDistance_))
	{
		// 一定の距離になったら停止状態へ
		stateTime_ = 0.0f;
		update_ = &EnemyBehavior::UpdateStop;

		// レーザーサイトを表示する
		laserSight_->On();

		//// 銃を構えるアニメーションをセット
		//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::AttackGun), 0.5f);
	}
	else if ((transform_->GetPos() - playerT_->GetPos()).SqMagnitude() >= Square(searchDistance_))
	{
		// 一定の距離以上になったら追跡前の状態へ
		stateTime_ = 0.0f;
		update_ = &EnemyBehavior::UpdateSearch;

		//// 待機状態のアニメーションをセット
		//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::Wait), 0.5f);
	}
	//}
}

// 停止状態の処理
void EnemyBehavior::UpdateStop(ObjectManager& objectManager, float delta)
{
	// 回転
	Rotation(delta);

	//if (!animetor_->GetAnimState()->IsBlend())
	//{
		// ブレンド終了時に来る

		// 射撃
		(this->*shot_)(objectManager, delta);
		if ((transform_->GetPos() - playerT_->GetPos()).SqMagnitude() >= Square(stopDistance_))
		{
			// 一定以上の距離になったら移動状態へ
			stateTime_ = 0.0f;
			update_ = &EnemyBehavior::UpdateMove;
			//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::WalkHaveGun), 0.5f);
			laserSight_->Off();
		}
	//}
}

// 攻撃を受けた後の状態
void EnemyBehavior::UpdateHit(ObjectManager& objectManager, float delta)
{
	//// 後ろに下がる
	//transform_->Pos() += knockBackVec_ * (speed_ * 0.5f * delta);
	//collider_->SetSpeed((delta * speed_));
	//collider_->SetMoveDir(knockBackVec_);

	if (stateTime_ >= hitTime_ )
	{
		// 硬直時間分経ったら

		// 時間をリセット
		stateTime_ = 0.0f;

		//// 移動に切り替え
		update_ = &EnemyBehavior::UpdateMove;
		DebugLog("硬直終了");
		// 3回攻撃を受けたら消滅
		if (damageCnt_ >= HpMax)
		{
			DebugLog("倒された");

			if (!isTutorial_)
			{
				// 近接攻撃中だったら削除する
				auto atkInfo = objectManager.GetComponent<ObjectInfo>(attackID_);
				if (atkInfo.IsActive() && *attackID_ != 0ull)
				{
					atkInfo->Destory();
				}
			}
			// 自信を破棄する
			objectManager.GetComponent<ObjectInfo>(ownerId_)->Destory();

			// エフェクトを出す
			auto id = objectManager.CreateFromFactory(FactoryID::FireExEffect, ownerId_, transform_->GetPos());
			objectManager.Begin(id);

			// SEを鳴らす
			lpSooundPross.PlayBackSound(SOUNDNAME_SE::enemyDestroy,false);
		}

		//// 武器切り替えアニメーション開始
		//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::GunToSword), 0.5f);

	}
}

//void EnemyBehavior::UpdateGunToSword(ObjectManager& objectManager, float delta)
//{
//	if (animetor_->GetAnimState()->IsEnd())
//	{
//		//// 近接攻撃のアニメーションをセット
//		//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::AttackSword), 0.5f);
//
//
//		//// 近接攻撃の時のものに変更
//		//update_ = &EnemyBehavior::UpdateAttackSword;
//
//		if (!isTutorial_)
//		{
//			// 近接攻撃のオブジェクトを生成
//			attackID_ = objectManager.CreateFromFactory(FactoryID::EnemyAttack, ownerId_, transform_->GetPos());
//			objectManager.GetComponent<EnemyAttackBehavior>(attackID_)->SetEnemyID(ownerId_);
//			objectManager.Begin(attackID_);
//		}
//	}
//}

//void EnemyBehavior::UpdateAttackSword(ObjectManager& objectManager, float delta)
//{
//	if (animetor_->GetAnimState()->IsEnd())
//	{
//		// アニメーション終了時
//
//		// 近接武器から銃に切り替えるアニメーションをする
//		animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::SwordToGun), 0.5f);
//
//		// アップデート関数を近接武器から銃に切り替える時のものに変更変更
//		update_ = &EnemyBehavior::UpdateSwordToGun;
//
//		if (!isTutorial_)
//		{
//			// 近接攻撃のオブジェクトの処理
//			auto info = objectManager.GetComponent<ObjectInfo>(attackID_);
//			if (info.IsActive() && *attackID_ != 0ull)
//			{
//				info->Destory();
//				DebugLog("攻撃終了", *attackID_);
//			}
//
//			// 無効なものにしておく
//			attackID_ = {};
//		}
//	}
//}

//void EnemyBehavior::UpdateSwordToGun(ObjectManager& objectManager, float delta)
//{
//	if (!animetor_->GetAnimState()->IsBlend() && animetor_->GetAnimState()->IsEnd())
//	{
//		// ブレンドもアニメーションも終了したとき
//
//		// 銃を構えたアニメーションをセット
//		animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::AttackGun), 0.5f);
//
//		// 停止状態のものに変更
//		update_ = &EnemyBehavior::UpdateStop;
//	}
//}

// 回転処理
void EnemyBehavior::Rotation(float delta)
{
	if (isRot_)
	{
		// 回転中だった時の処理
		transform_->AddRotation({ 0.0f, Deg2Rad(60.0f) * rotSing_ * delta, 0.0f });
	}

	// ターゲットまでのベクトルを求める
	auto targetVec{ playerT_->GetPos() - transform_->GetPos()};

	// 前方方向を取得
	auto forward{ transform_->GetForward() };

	// 回転すべき角度を求める
	auto rot = Square(std::atan2(targetVec.z, targetVec.x) - std::atan2(forward.z, forward.x));
	if (rot > rotDeff)
	{
		// 回転すべき時
		isRot_ = true;
		rotSing_ = Cross(Vector2{ forward.z, forward.x }, Vector2{ targetVec.z,targetVec.x }) > 0.0f ? 1.0f: -1.0f;
	}
	else
	{
		isRot_ = false;
	}
}

//// 重力処理
//void EnemyBehavior::Gravity(float delta)
//{
//	if (!collider_->IsGround())
//	{
//		// 空中にいる時
//		jumpTime_ += delta;
//		float val = (v0 *jumpTime_) - (0.5f * gravity * Square(jumpTime_));
//		transform_->Pos() += transform_->GetUp() * (val);
//		collider_->SetGravityPow(val);
//	}
//	else
//	{
//		jumpTime_ = 0.0f;
//	}
//}


// ヒット時の処理
void EnemyBehavior::OnHit(Collider& col, ObjectManager& objectManager)
{
	auto atr = objectManager.GetComponent<ObjectInfo>(col.GetOwnerID())->GetAttribute();
	if (atr == ObjectAttribute::PlayerAttack)
	{
		auto player = (objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID()));
		if (!player.IsActive())
		{
			// プレイヤーがアクティブ出ないとき処理しない
			return;
		}

		if (damageCnt_ >= HpMax)
		{
			// もうすでに死亡しているので処理しない
			return;
		}
		auto power = objectManager.GetComponent<PlayerBehavior>(objectManager.GetPlayerID())->GetPlayerPower();
		damageCnt_ += power;
		player->RiseSkillValue();
		lpSooundPross.PlayBackSound(SOUNDNAME_SE::playerAttackHit, false);

		// 当たり判定との中点にエフェクトを生成する
		auto hit = objectManager.GetComponent<Transform>(col.GetOwnerID());
		auto effect = objectManager.CreateFromFactory(FactoryID::HitEffect, ownerId_ ,(hit->GetPos() + transform_->GetPos()) / 2.0f);
		objectManager.GetComponent<Transform>(effect)->Scale() = Vector3{ 0.5f,0.5f,0.5f };
		objectManager.Begin(effect);
		// 攻撃を受けた時のものに変更
		update_ = &EnemyBehavior::UpdateHit;

	}
	else if (atr == ObjectAttribute::PlayerSkill)
	{
		if (damageCnt_ >= HpMax)
		{
			// もうすでに死亡しているので処理しない
			return;
		}
		// 一撃撃破
		damageCnt_ += HpMax;
		// 攻撃を受けた時のものに変更
		update_ = &EnemyBehavior::UpdateHit;

	}
}

void EnemyBehavior::KnockBack(ObjectManager& objectManager)
{
	// 攻撃を受けた時のものに変更
	update_ = &EnemyBehavior::UpdateHit;
	stateTime_ = 0.0f;

	//// ジャンプさせる
	//jumpTime_ = 0.0f;
	collider_->SetGroundFlag(false);
	transform_->Pos().y += 2.0f;

	// 死亡していたらヒットストップを多めにする
	objectManager.StartHitStop(damageCnt_ >= HpMax ? 0.12f : 0.05f);

	// プレイヤーの座標からノックバックすべき方向を求める
	auto playerTransform = (objectManager.GetComponent<Transform>(objectManager.GetPlayerID()));
	if (playerTransform.IsActive())
	{
		knockBackVec_ = (transform_->GetPos() - playerTransform->GetPos());
		knockBackVec_.y = 0.0f;
		knockBackVec_.Normalize();
	}

	//// 攻撃を受けた時のアニメーションをセット
	//animetor_->SetNextAnimation(static_cast<int>(EnemyAnim::GetHit), 0.125f);

	// 攻撃を受けた時のSEを鳴らす
	lpSooundPross.PlayBackSound(SOUNDNAME_SE::playerAttackHit,false);
}
