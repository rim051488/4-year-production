#pragma once
#include <map>
#include <DxLib.h>
#include "UiBase.h"
#include "../Common/Math.h"
#include "../Object/ObjectManager.h"

//	敵との距離と角度を格納
struct MiniMapRadar
{
	float angle;
	float range;
	float flag;
};

class MiniMap :
	public UiBase
{
	// ミニマップに敵を表示する範囲
	static constexpr float RADAR_RANGE = 270;
	static constexpr float RADAR_DISTANSE = 5;
public:
	MiniMap(const std::filesystem::path& path);


	void Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller) final;
	void Draw(int mainScr) final;
private:
	UiID GetUIID(void) final { return UiID::MinMap; }
	// プレイヤーとエネミーの距離と角度をエネミーの数だけ取得
	std::map<int, MiniMapRadar> miniMap_;

	// mapにどれだけ格納したかを取得
	int count_;

	// ミニマップのサイズ
	int miniMapSize_;
	// 描画するスクリーンの作成
	int MapScr_;
	// 画像の描画
	int MapGraph_;

};

