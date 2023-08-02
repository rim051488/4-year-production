#pragma once
#include "Factory.h"
#include <string>
#include <fstream>
#include "../Common/Vector3.h"
#include "../Common/SharedHandle.h"

// 敵を生成するクラス
class EnemyFactory :
	public Factory
{
public:
	EnemyFactory(ObjectID& stageID,const std::string& path, ObjectManager& objectManager);
private:
	ObjectID Create(ObjectID ownerID, const Vector3& pos, const Vector3& rot = { 0.0f,0.0f,0.0f }) final;
	const FactoryID GetID(void) const { return FactoryID::Enemy; }
	void LoadTrans(std::ifstream& file, std::uint32_t size) final;
	void LoadModel(std::ifstream& file, std::uint32_t size) final;
	void LoadCollider(std::ifstream& file, std::uint32_t size) final;
	void LoadOther(std::ifstream& file, std::uint32_t size) final;

	// 回転
	Vector3 rot_;

	// 拡縮
	Vector3 scale_;

	// モデルのパス
	std::string modelPath_;

	// あらかじめ読み込んでおくために使う
	SharedModelHandle preLoadHandle_;

	// 当たり判定の中央
	Vector3 colCeter_;

	// 当たり判定の高さ
	float height_;

	// 当たり判定の半径
	float radius_;

	// 各パラメータ
	float hitTime_;
	float searchDist_;
	float speed_;
	float stopDist_;

	float shotInterval_;
	float burstInterval_;
	float burstNum_;

	// ステージのID
	ObjectID stageID_;
};

