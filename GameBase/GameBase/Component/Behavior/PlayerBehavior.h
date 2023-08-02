#pragma once
#include <map>
#include <string>
#include <vector>
#include "../../Object/ObjectManager.h"
#include "../Transform/Transform.h"
#include "../Collider/CharactorCollider.h"
#include "../../Common/Input/Controller.h"
#include "Behavior.h"
#include "../Render/ModelRender.h"
#include "../Animator/Animator.h"
#include "../../UI/UiID.h"
#include "../../Common/SoundPross.h"
#include "../Animator/PlayerAnimIndex.h"
#include "../../Factory/FactoryID.h"
#include "ThrusterBehavior.h"
#include "SandSmokeBehavior.h"
#include "SandSmokeSlowMoveBehavior.h"
#include "SandSmokeDiffusionBehavior.h"

class PlayerBehavior :
    public Behavior
{
public:
    // 射撃用データ
    struct ShotData
    {
        float shotInterval;             // 撃つまでのインターバル
        float burstInterval;            // バースト射撃の一発当たりのインターバル
        int burstNum;                   // バースト射撃の発射数
    };

    PlayerBehavior(void);

    void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

    ComponentID GetID(void) const override
    {
        return id_;
    }
    static constexpr ComponentID id_{ ComponentID::PlayerBehavior };

    // ゲージ情報取得
    std::pair<float, std::pair<float, float>> GetGaugeValue(UiID id) const { return gauge_.at(id); }
    // 攻撃数取得
    int GetAtkCnt(void) { return atkCnt_; }
    // コンボ数取得
    int GetComboNum(void) { return combo_; }
    // コンボ数加算
    void AddCombo(void) { combo_++; }
    // スキルゲージ上昇
    void RiseSkillValue(void);
    // スキル攻撃終了
    void EndSkill(void) { isSkill_ = false; }
    // 現在回避中か
    bool IsDodge(void) { return isDodge_; }
    // 現在ダッシュ中か
    bool IsDash(void) { return isDash_; }
    /// <summary>
    /// 射撃データをセットする
    /// </summary>
    /// <param name="data"></param>
    void SetShotData(const ShotData& data)
    {
        shotData_ = data;
    }
    int GetPlayerPower(void) { return Power; }

private:
    // 攻撃力
    static constexpr int Power = 1;

    // 初期化
    bool Init(void);
    void Begin(ObjectManager& objectManager) final;
    /// <summary>
    /// 弾の発射処理
    /// </summary>
    /// <param name="objectManager"> オブジェクトマネージャー </param>
    /// <param name="delta"> デルタタイム </param>
    void Shot(ObjectManager& objectManager, float delta);

    //// 更新
    //void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
    // 移動制御
    float ProcessMove(BaseScene& scene, ObjectManager& objectManager, Controller& controller, float speed, float delta);
    // 移動制限
    void StrictionMove(void);
    // 必殺発動時に後に下がる
    void ProcessMoveSkill(float interval, float delta);
    // ダッシュ制御
    float ProcessDash(Controller& controller, const float& delta);
    // 回転制御
    void ProcessRotate(ObjectManager& objectManager, BaseScene& scene, Controller& controller, float rot);
    //// ジャンプ制御
    //void ProcessJump(Controller& controller);
    //// 浮遊制御
    //void ProcessFloat(void);
    // 攻撃制御
    void ProcessAttack(Controller& controller, ObjectManager& objectManager,float delta);
    // スタミナ制御
    void ProcessStamina(Controller& controller);
    // クールタイム計測
    void CoolTimer(float& delta);
    // スキル制御
    void ProcessSkill(Controller& controller, ObjectManager& objectManager);
    //// アニメーション制御
    //void Animation(Controller& controller, ObjectManager& objectManager);
    // 音関係制御
    void Sound(void);
    // 衝突処理
    void OnHit(Collider& col, ObjectManager& objectManager);
    // 地面の高さ調整
    //void CorrectionGroundPos(void);
    // コンボリセット
    void ResetCombo(void);


    // 回避継続時間
    static constexpr float DodgeLimit = 0.5f;
    // スタミナ切れクールタイム
    static constexpr float CoolTime = 3.0f;
    // 徐々に回転する量
    static constexpr float SpeedRotRad = 5.0f * (DX_PI_F / 180.0f);
    // ゲージ上昇定数
    static constexpr float RiseValue = 0.25f;
    // 攻撃アニメーション最大番号
    static constexpr int AtkAnimMax = 3;
    // ホバリングY座標最高値
    static constexpr float HoverPosYMax = 1000.0f;
    // 浮遊Y座標最高値
    static constexpr float FloatPosYMax = 8.0f;
    // プレイヤーの下方向Offset（仮）
    static constexpr float playerDownOffset = 80.0f;

    // 移動量
    struct moveDis {
        float walk = 30.0f;         // 歩き
        float dash = 60.0f;         // ダッシュ
        float dodge = 100.0f;        // 回避
    };
    // 方向角度
    struct dir {
        float left = 90.0f;         // 左
        float right = 270.0f;       // 右
        float up = 180.0f;          // 上
        float down = 0.0f;          // 下
    };
    //// ジャンプ
    //struct jump {
    //    float initVel = 10.0f;      // 初速度
    //    float vel = 0.0f;           // y方向の速度
    //    float acc = 1.0f;           // 重力加速度
    //};
    //// 浮遊
    //struct floating {
    //    float initVel = 10.0f;      // 初速度
    //    float vel = 0.0f;           // y方向の速度
    //    float acc = 0.01f;          // 重力加速度
    //    float groundPosY = 0.0f;    // 地面のY座標
    //    bool isUp = true;           // 上昇中か
    //    bool canFloat = true;       // 実行可能か
    //};
    //// アニメーション再生時間
    //struct animTime {
    //    float idle = 0.1f;          // 立ち
    //    float move = 0.1f;          // 移動
    //    float attack1 = 0.3f;       // 1撃目
    //    float attack2 = 0.1f;       // 2撃目
    //    float attack3 = 0.3f;       // 3撃目
    //    float skill = 0.5;
    //};

    //// アニメーション
    //ComponentHandle<Animator> animation_;
    // トランスフォーム
    ComponentHandle<Transform> transform_;
    // プレイヤーのトランスフォーム
    ComponentHandle<Transform> enemyT_;

    // コリジョン
    ComponentHandle<CharactorCollider> collider_;

    // ブラスターエフェクト
    ComponentHandle<ThrusterBehavior> blaster_;
    // 砂煙エフェクト
    ComponentHandle<SandSmokeBehavior> sand_;
    // 砂煙（広がり）エフェクト
    ComponentHandle<SandSmokeDiffusionBehavior> sandDiff_;
    // 砂煙(遅い時用)
    ComponentHandle<SandSmokeSlowMoveBehavior> sandSlow_;


    // カメラのトランスフォーム
    ComponentHandle<Transform> camera_;

    // 移動量
    moveDis movedis_;
    // ダッシュし始めから経過時間
    float dodgeTimer_;
    // 回避中か
    bool isDodge_;
    // 現在ダッシュ中か
    bool isDash_;
    // ダッシュ禁止時間
    float noDashTime_;
    // スタミナ切れになったか
    bool isStaminaLoss_;
    // 方向角度
    dir dir_;
    // 回転情報
    Vector3 angles_;
    // 射撃時間
    float shotTime_;

    // バースト射撃回数
    int burstCnt_;
    // 射撃に関するデータ
    ShotData shotData_;

    // 攻撃中か
    bool atkLimit_;
    // 攻撃アニメーション中か
    bool isAttack_;
    // 攻撃回数
    int atkCnt_;
    // コンボ数
    int combo_;
    // スキル使用可能か
    bool canSkill_;
    // スキル使用中か
    bool isSkill_;

    //// アニメーション情報
    //std::vector<std::pair<AnimIndex, float>> anim_;
    //// アニメーションごとの再生時間
    //animTime animtime_;

    // 射撃処理
    void (PlayerBehavior::* shot_)(ObjectManager&, float);

    // ゲージ値情報
    std::map<UiID, std::pair<float, std::pair<float, float>>> gauge_;

    // サウンド
    std::vector<std::pair<SOUNDNAME_SE, bool>> sound_;
};

