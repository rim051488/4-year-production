#pragma once

// オブジェクトの属性
enum class ObjectAttribute
{
	Player,					// プレイヤー
	PlayerAttack,			// プレイヤーの攻撃範囲
	PlayerSkill,			// プレイヤーの必殺技範囲
	Enemy,					// 敵
	EnemyAttack,			// 敵の攻撃範囲
	Stage,					// ステージ
	Sky,					// 背景の空
	Other					// それ以外
};
