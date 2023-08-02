#pragma once
#include <filesystem>
#include "Factory.h"
#include <atomic>
#include <map>
#include "../Common/Vector2.h"

// ステージを作成するクラス
class StageFactory :
    public Factory
{
public:
	StageFactory(const std::filesystem::path& path, ObjectManager& objectManager);
	StageFactory(ObjectManager& objectManager);
private:
	const FactoryID GetID(void) const { return FactoryID::Stage; }
	
	/// <summary>
	/// ロード用関数
	/// </summary>
	/// <param name=""></param>
	void Load(void);

	/// <summary>
	/// チュートリアルロード用
	/// </summary>
	/// <param name=""></param>
	void LoadTutorial(void);

	/// <summary>
	/// ロード完了したか
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const bool IsLoaded(void) final;

	/// <summary>
	/// 背景のスカイドームの生成
	/// </summary>
	/// <param name="file"></param>
	void CreateSkyDome(std::ifstream& file);
	
	/// <summary>
	/// プレイヤーの生成
	/// </summary>
	/// <param name="file"></param>
	void CreatePlayer(std::ifstream& file);

	/// <summary>
	/// 敵の生成
	/// </summary>
	/// <param name="file"></param>
	void CreateEnemy(std::ifstream& file) ;

	void LoadTrans(std::ifstream& file, std::uint32_t size) final;

	void LoadModel(std::ifstream& file, std::uint32_t size) final;

	void LoadSpawn(std::ifstream& file, std::uint32_t size) final;

	void LoadCollider(std::ifstream& file, std::uint32_t size) final;

	// ロード完了したかのフラグ
	std::atomic_bool isLoaded_;

	// データファイルのパス
	std::filesystem::path path_;

	// ステージの当たり判定のファイル名
	std::string stageCol_;

	// ステージのオブジェクトID
	ObjectID id_;


	// モデルのファイル名
	std::string model_;

	Vector3 pos_;
	Vector3 scale_;


	// 関数のマップ
	std::map<FactoryID, void(StageFactory::*)(std::ifstream&)> funcMap_;

	std::map<size_t, Vector2> enemyPosList_;
};

