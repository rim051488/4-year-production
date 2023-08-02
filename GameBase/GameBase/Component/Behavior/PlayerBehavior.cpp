#include "../../Scene/BaseScene.h"
#include "PlayerBehavior.h"
#include "../../Common/Input/InputType/Keyboard.h"
#include "../../Common/Input/InputType/Pad.h"
#include "../../Common/Debug.h"
#include "../../Common/Math.h"
#include "../../Common/Camera.h"
#include "../../Common/SoundPross.h"
#include "../Collider/CharactorCollider.h"
#include "../Info/ObjectInfo.h"
#include "EnemyBehavior.h"
#include "PlayerAttackBehavior.h"
#include "PlayerSkillBehavior.h"
#include "CameraBehavior.h"

PlayerBehavior::PlayerBehavior(void)
{
	Init();
}

bool PlayerBehavior::Init(void)
{
	shotTime_ = 0.0f;
	// 移動
	dodgeTimer_ = 0.0f;
	noDashTime_ = 0.0f;
	isStaminaLoss_ = false;
	// 回転
	angles_ = Vector3{ Deg2Rad(0.0f),Deg2Rad(dir_.up),Deg2Rad(0.0f) };
	// 攻撃
	atkLimit_ = false;
	isAttack_ = false;
	isDodge_ = false;
	combo_ = 0;
	canSkill_ = false;
	isSkill_ = false;

	// ゲージ
	gauge_.emplace(UiID::Hp, std::pair(200.0f, std::pair(0.0f, 200.0f)));
	gauge_.emplace(UiID::Skill, std::pair(0.0f, std::pair(0.0f, 200.0f)));
	gauge_.emplace(UiID::Stamina, std::pair(200.0f, std::pair(0.0f, 200.0f)));
	atkCnt_ = 0;
	// サウンド
	for (int i = 0; i < static_cast<int>(SOUNDNAME_SE::Max); i++)
	{
		sound_.push_back(std::pair(static_cast<SOUNDNAME_SE>(i), false));
	}
	
	return true;
}

void PlayerBehavior::Begin(ObjectManager& objectManager)
{
	// Y座標地面固定
	transform_ = objectManager.GetComponent<Transform>(ownerId_);
	// モデル情報
	auto modelrender = objectManager.GetComponent<ModelRender>(ownerId_);
	auto& handle = modelrender->GetHandle();

	// トランスフォームを取得
	transform_ = objectManager.GetComponent<Transform>(ownerId_);

	// プレイヤーのトランスフォームを取得
	enemyT_ = objectManager.GetComponent<Transform>(objectManager.GetPlayerID());

	// コライダーを取得しヒット時の関数をセット
	collider_ = objectManager.GetComponent<CharactorCollider>(ownerId_);
	collider_->SetHitFunc(std::bind(&PlayerBehavior::OnHit, this, std::placeholders::_1, std::placeholders::_2));
	collider_->SetGroundFlag(true);

	shot_ = &PlayerBehavior::Shot;
	// エフェクト
	// ブラスター
	auto blasterID = objectManager.CreateFromFactory(FactoryID::BlasterEffect, ownerId_, transform_->GetPos());
	objectManager.Begin(blasterID);
	blaster_ = objectManager.GetComponent<ThrusterBehavior>(blasterID);
	// 砂煙
	auto sandID = objectManager.CreateFromFactory(FactoryID::SandSmokeEffect, ownerId_, transform_->GetPos());
	objectManager.Begin(sandID);
	sand_ = objectManager.GetComponent<SandSmokeBehavior>(sandID);
	// 砂煙（広がり）エフェクト
	auto sandDiffID = objectManager.CreateFromFactory(FactoryID::SandSmokeDiffusionEffect, ownerId_, transform_->GetPos());
	objectManager.Begin(sandDiffID);
	sandDiff_ = objectManager.GetComponent<SandSmokeDiffusionBehavior>(sandDiffID);
	sandDiff_->Play();
	// 砂煙(移動遅い時用)
	auto sandSlowID = objectManager.CreateFromFactory(FactoryID::SandSmokeSlowMoveEffect, ownerId_, transform_->GetPos());
	objectManager.Begin(sandSlowID);
	sandSlow_ = objectManager.GetComponent<SandSmokeSlowMoveBehavior>(sandSlowID);
	// カメラ
	camera_ = objectManager.GetComponent<Transform>(objectManager.GetCameraID());
	//// 地面座標
	//float_.groundPosY = transform_->Pos().y;


	lpSooundPross.PlayBackSound(SOUNDNAME_SE::playerMove,true);
}

void PlayerBehavior::Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller)
{
	// 操作制御
	auto speed = ProcessDash(controller, delta);
	collider_->SetSpeed(speed);
	auto rot = ProcessMove(scene, objectManager, controller, speed, delta);
	ProcessRotate(objectManager, scene, controller, rot);
	ProcessAttack(controller, objectManager,delta);
	ProcessStamina(controller);
	if (isStaminaLoss_)
	{
		// クールタイム
		CoolTimer(delta);
	}
	ProcessSkill(controller, objectManager);
	// 音関係制御
	Sound();
}

void PlayerBehavior::Shot(ObjectManager& objectManager, float delta)
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
			auto id = objectManager.CreateFromFactory(FactoryID::PlayerBullet, ownerId_, shotPos);
			objectManager.Begin(id);
			// マズルフラッシュエフェクト
			auto effect = objectManager.CreateFromFactory(FactoryID::MuzzleFlashEffect, ownerId_, shotPos, { 0.0f,transform_->GetRotation().ToEuler().y + Deg2Rad(180.0f), 0.0f });
			objectManager.GetComponent<Transform>(effect)->Scale() = Vector3{ 0.5f,0.5f,0.5f };
			objectManager.Begin(effect);

			if (burstCnt_ >= shotData_.burstNum)
			{
				// 規定の数発射したら0にする
				burstCnt_ = 0;
				shotTime_ = -shotData_.shotInterval;
			}
		}
	}
}

float PlayerBehavior::ProcessMove(BaseScene& scene, ObjectManager& objectManager, Controller& controller, float speed, float delta)
{
	// 移動キー入力判定
	auto rot = controller.GetLeftInput().GetAngle();
	// 必殺技発動中は移動自体をさせない
	if (isSkill_)
	{
		ProcessMoveSkill(1.0f, delta);
		return rot;
	}

	// 通常移動
	if (controller.GetLeftInput().SqMagnitude() > 0.0f)
	{
		auto camAngle = camera_->GetRotation().ToEuler();
		transform_->Pos().x -= sinf(camAngle.y + rot) * speed;
		transform_->Pos().z -= cosf(camAngle.y + rot) * speed;
		// 移動中は浮遊不可
		//float_.canFloat = false;
		sandDiff_->Play();
	}
	else
	{
		// 浮遊可
		//float_.canFloat = true;
		sandSlow_->Play();
	}
	StrictionMove();
	return rot;
}

void PlayerBehavior::StrictionMove(void)
{
	if (transform_->Pos().z <= -18000)
	{
		transform_->Pos().z = -18000;
	}
	if (transform_->Pos().z >= 18000)
	{
		transform_->Pos().z = 18000;
	}
	if (transform_->Pos().x <= -16000)
	{
		transform_->Pos().x = -16000;
	}
	if (transform_->Pos().x >= 16000)
	{
		transform_->Pos().x = 16000;
	}

}

void PlayerBehavior::ProcessMoveSkill(float interval, float delta)
{
	if (!blaster_->IsPlay())
	{
		return;
	}
	// ビームの反動で後に下がる
	transform_->Pos().x += sinf(angles_.y);
	transform_->Pos().z += cosf(angles_.y);
}

float PlayerBehavior::ProcessDash(Controller& controller, const float& delta)
{
	auto speed = movedis_.walk;
	// スタミナ切れしていたらぬける
	if (isStaminaLoss_)
	{
		return speed;
	}

	// 回避制御
	if (controller.Pressed(InputID::Dash))
	{
		// ダッシュ中
		isDodge_ = true;
		isDash_ = true;
		// ダッシュした地点でのエフェクト
		blaster_->Play();
		sand_->Play();
		sandDiff_->Stop();
		sandSlow_->Stop();
		// サウンド
		sound_.at(static_cast<int>(SOUNDNAME_SE::playerDash)).second = true;
		lpSooundPross.PlayBackSound(SOUNDNAME_SE::playerAB,true);
	}
	else if (dodgeTimer_ > DodgeLimit)
	{
		// 回避終わり
		isDodge_ = false;
	}
	else if (controller.Released(InputID::Dash))
	{
		// ダッシュ終わり(回避も含む)
		isDodge_ = false;
		isDash_ = false;
	}
	// ダッシュ制御
	if (controller.Press(InputID::Dash) && (controller.GetLeftInput().SqMagnitude() > 0.0f))
	{
		speed = isDodge_ ? movedis_.dodge : movedis_.dash;									// 回避の有無による移動量計算
		(dodgeTimer_ >= 0.0f && isDodge_) ? dodgeTimer_ += delta : dodgeTimer_ = 0.0f;		// 回避時間中及び、回避中ならタイマー経過
	}
	else if (controller.Released(InputID::Dash))
	{
		// エフェクト
		blaster_->Stop();
		sand_->Stop();
		sandSlow_->Play();
		// サウンド
		lpSooundPross.SoundStop(SOUNDNAME_SE::playerAB);
	}
	return speed;
}

void PlayerBehavior::ProcessRotate(ObjectManager& objectManager, BaseScene& scene, Controller& controller, float rot)
{
	// モデルの角度を設定
	transform_->SetRotation({ angles_.x, angles_.y, angles_.z });
	if (!(controller.GetLeftInput().SqMagnitude() > 0.0f))
	{
		return;
	}

	// 移動方向にキャラクターの角度を徐々に変える
	float radUnitAnglesY = angles_.y;
	auto camAngle = camera_->GetRotation().ToEuler();

	float radMoveAnglesY = RadIn2PI(camAngle.y + rot);

	// 回転量が少ない方の回転方向を取得する（時計回り：１、反時計回り：ー１）
	float aroundDir = DirNearAroundRad(radUnitAnglesY, radMoveAnglesY);

	// true:しきい値以内に合わせる false:回転量を加える
	float diff = abs(radMoveAnglesY - radUnitAnglesY);
	auto rotRad = (isSkill_) ? SpeedRotRad / 10 : SpeedRotRad;
	angles_.y = (diff < 0.1f && !isSkill_) ? radMoveAnglesY : angles_.y + (rotRad * aroundDir);

	// angles_.yも0~360度以内に収める
	angles_.y = RadIn2PI(angles_.y);
	collider_->SetMoveDir(-transform_->GetForward());
}

void PlayerBehavior::ProcessAttack(Controller& controller, ObjectManager& objectManager, float delta)
{
	if (controller.Pressed(InputID::Attack) && !isSkill_ && !isAttack_)
	{
		Shot(objectManager,delta);
	}
}

void PlayerBehavior::ResetCombo(void)
{
	atkCnt_ = 0;
	combo_ = 0;
	atkLimit_ = false;
}

void PlayerBehavior::ProcessStamina(Controller& controller)
{
	auto& value = gauge_.at(UiID::Stamina);
	auto& [min, max] = value.second;
	auto dashFlag = (controller.Press(InputID::Dash) && controller.GetLeftInput().SqMagnitude() > 0.0f && !isStaminaLoss_);
	// ダッシュ
	value.first = (dashFlag && isDodge_) ? value.first - RiseValue				// 回避
		: dashFlag ? value.first - RiseValue * 0.5f								// ダッシュ
		: value.first;															// 回復

	// ダッシュ、ジャンプをしていなければスタミナ回復
	if ((!controller.Press(InputID::Jump) && !(controller.Press(InputID::Dash) && (controller.GetLeftInput().SqMagnitude() > 0.0f))) || isStaminaLoss_)
	{
		value.first += RiseValue;
	}
	value.first = Clamp(value.first, min, max);
	// スタミナ切れ
	if (value.first <= min)
	{
		isStaminaLoss_ = true;
		isDodge_ = false;
		isDash_ = false;
		// エフェクト
		blaster_->Stop();
		sand_->Stop();
		// サウンド
		lpSooundPross.SoundStop(SOUNDNAME_SE::playerAB);
	}
}

void PlayerBehavior::CoolTimer(float& delta)
{
	// クールタイム
	noDashTime_ += delta;
	if (noDashTime_ >= CoolTime)
	{
		isStaminaLoss_ = false;
		noDashTime_ = 0.0f;
	}
}

void PlayerBehavior::ProcessSkill(Controller& controller, ObjectManager& objectManager)
{
	if (controller.Pressed(InputID::Skil) && canSkill_)
	{
		// 必殺スキル
		auto id = objectManager.CreateFromFactory(FactoryID::PlayerSkill, ownerId_, transform_->GetPos());
		objectManager.Begin(id);

		auto skill = objectManager.GetComponent<PlayerSkillBehavior>(id);
		skill->SetAttackStartTime(2.4f);
		isSkill_ = true;
		// ブラスター発動
		blaster_->Play(2.4f);
		sandSlow_->Play(2.4f);
		sandDiff_->Stop();
		sand_->Stop();
		// ゲージリセット
		gauge_.at(UiID::Skill).first = 0.0f;
		canSkill_ = false;
		sound_.at(static_cast<int>(SOUNDNAME_SE::playerSpetial)).second = true;
	}

}

void PlayerBehavior::RiseSkillValue(void)
{
	auto& value = gauge_.at(UiID::Skill);
	auto& [min, max] = value.second;
	// スキルゲージ上昇
	value.first = Clamp(value.first + RiseValue * 30.0f, min, max);
	// ゲージMAX
	if (value.first >= max)
	{
		canSkill_ = true;
	}
}

void PlayerBehavior::Sound(void)
{
	// 音制御
	for (auto& s : sound_)
	{
		if (s.second)
		{
			lpSooundPross.PlayBackSound(s.first,false);
			s.second = false;
		}
	}
}

void PlayerBehavior::OnHit(Collider& col, ObjectManager& objectManager)
{
	auto atr = objectManager.GetComponent<ObjectInfo>(col.GetOwnerID())->GetAttribute();
	if (atr == ObjectAttribute::EnemyAttack)
	{
		// 画面揺れ（カメラシェイク）
		auto camera = objectManager.GetComponent<CameraBehavior>(objectManager.GetCameraID());
		if (camera.IsActive())
		{
			camera->StartShake(0.1f);
		}

		// 攻撃エフェクト
		auto attackTransform = objectManager.GetComponent<Transform>(col.GetOwnerID());
		auto effect = objectManager.CreateFromFactory(FactoryID::HitEffect, ownerId_, (transform_->GetPos()+ attackTransform->GetPos())/2.0f);
		objectManager.GetComponent<Transform>(effect)->Scale() = Vector3{ 0.3f,0.3f ,0.3f };
		objectManager.Begin(effect);

		// HP減らす
		auto power = objectManager.GetComponent<EnemyBehavior>(objectManager.GetEnemyID())->GetEnemyPower();
		auto& value = gauge_.at(UiID::Hp);
		auto& [min, max] = value.second;
		// HP消費
		value.first = Clamp(value.first - power, min, max);
		// HP全損
		if (value.first <= 0.0f)
		{
			// プレイヤー消滅処理
			objectManager.GetComponent<ObjectInfo>(objectManager.GetPlayerID())->Destory();
			sound_.at(static_cast<int>(SOUNDNAME_SE::playerDestory)).second = true;
		}
		else
		{
			sound_.at(static_cast<int>(SOUNDNAME_SE::playerHit)).second = true;
		}
	}
}
