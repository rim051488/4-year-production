#pragma once

enum class FactoryID
{
	Stage,		// ステージ
	Sky,		// 背景
	Player,		// プレイヤー
	Enemy,		// 敵
	EnemyBullet,				// 敵の弾
	PlayerAttack,				// 攻撃判定
	PlayerSkill,				// プレイヤー必殺技
	PlayerBullet,				// プレイヤーの弾
	Tutorial,
	FireExEffect,				// 敵の爆発（ショート）
	HorizontalEffect,			// 水平切り
	RedExEffect,				// 敵の爆発（ロング）
	ThrustEffect,				// 突き
	VerticalEffect,				// 垂直切り
	BeemEffect,					// 必殺スキル（ビーム）
	BlasterEffect,				// 飛行用ブラスター
	HitEffect,					// 当たりエフェクト
	SandSmokeEffect,			// 砂煙エフェクト
	SandSmokeDiffusionEffect,	// 砂煙（広がり）エフェクト
	SandSmokeSlowMoveEffect,
	MuzzleFlashEffect,			// マズルフラッシュ
	EnemyAttack,				// 敵の近接攻撃
	Max
};
