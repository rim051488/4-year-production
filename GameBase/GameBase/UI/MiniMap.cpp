#include "MiniMap.h"
#include "../Component/Transform/Transform.h"
#include "../Component/Behavior/StageBehavior.h"
MiniMap::MiniMap(const std::filesystem::path& path) : UiBase{zeroVector2<float>}
{
	count_ = 0;
	int x, y;
	GetDrawScreenSize(&x, &y);
	miniMapSize_ = y / 4;
	MapScr_ = MakeScreen(miniMapSize_, miniMapSize_);
	MapGraph_ = LoadGraph(path.c_str());
}

void MiniMap::Update(float delta, BaseScene& scene, ObjectManager& objMng, Controller& controller)
{
	// プレイヤーの座標を取得
	auto player = objMng.GetComponent<Transform>(objMng.GetPlayerID());
	auto& pos = player->GetPos();
	auto pPos_ = Vector2(pos.x, pos.z);
	// エネミーの情報を取得
	auto stage = objMng.GetComponent<StageBehavior>(objMng.GetStageID());
	auto& list = stage->GetEnemyPosList();
	int i = 0;
	for (auto& enemy : list)
	{
		MiniMapRadar miniMap;
		// プレイヤーとの距離を取得
		Vector2 tmp =  Vector2( (enemy.second.x - pPos_.x) / RADAR_DISTANSE, (enemy.second.y - pPos_.y) / RADAR_DISTANSE);
		miniMap.range = tmp.Magnitude();
		// カメラの情報を取得
		auto camera = objMng.GetComponent<Transform>(objMng.GetCameraID());
		// カメラの向いている方向からの角度を求める
		Vector2 camDir = Vector2(camera->GetForward().x, camera->GetForward().z);
		// 角度を求める
		miniMap.angle = GetAngle2Vector(camDir, tmp);
		// デフォルトは表示しない
		miniMap.flag = false;
		// 距離が表示可能範囲内であれば表示する
		if (miniMap.range <= (RADAR_RANGE * RADAR_RANGE))
		{
			miniMap.flag = true;
		}
		// 敵が追加で出現した場合は追加する(現状はチュートリアルのみ)
		if (miniMap_.size() < list.size())
		{
			miniMap_.emplace(i, miniMap);
			count_++;
		}
		else
		{
			miniMap_.at(i) = miniMap;
		}
		i++;
	}
	// 敵が死亡するごとにmapの要素を消去する
	if (i < count_)
	{
		count_--;
		miniMap_.erase(count_);
	}
}

void MiniMap::Draw(int mainScr)
{
	// レーダーのようなミニマップを作成
	SetDrawScreen(MapScr_);
	ClsDrawScreen();
	DrawGraph(0, 0, MapGraph_, true);

	// 中心位置の取得
	int SizeHalf = miniMapSize_ / 2;
	// プレイヤーの位置
	DrawCircle(SizeHalf, SizeHalf, 5, 0x0000ff);

	for (const auto& map : miniMap_)
	{
		if (map.second.flag)
		{
			Vector2 pos = Vector2(0, -1);
			Vector2 tmp;
			// 受け取った角度でsinとcosの値をだす
			float sinParam = sin(-map.second.angle);
			float cosParam = cos(-map.second.angle);
			// sinParamとcosParamで敵の位置を取得する
			tmp.x = (pos.x * cosParam) - (pos.y * sinParam);
			tmp.y = (pos.x * sinParam) + (pos.y * cosParam);
			// 表示位置を補正する(0,0の位置からになるので中心からの位置にする)
			pos.x = tmp.x * (map.second.range / 50) + SizeHalf;
			pos.y = tmp.y * (map.second.range / 50) + SizeHalf;
			DrawCircle(static_cast<int>(pos.x), static_cast<int>(pos.y), 2, 0xff0000);
		}
	}
	SetDrawScreen(mainScr);
	// 作成したミニマップをメインスクリーンに描画
	DrawGraph(10, 10, MapScr_, true);
}
