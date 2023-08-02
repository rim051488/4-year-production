#pragma once
// アニメーション名
enum class AnimIndex {
    Idle,                       // 立ち
    MoveSlow,                   // 低速移動
    ForwardMove,                // 前方移動中
    Attack1,                    // 攻撃1コンボ目
    Attack2,                    // 攻撃2コンボ目
    Attack3,                    // 攻撃3コンボ目
    MoveAttack1,                // 移動攻撃1コンボ目
    MoveAttack2,                // 移動攻撃2コンボ目
    Rool,                       // ロール
    Skill,                      // スキル
    Max,
};
