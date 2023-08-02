#pragma once
#include <fstream>
#include "../Object/ObjectID.h"
#include "FactoryID.h"
#include "../Common/Vector3.h"

class ObjectManager;

// オブジェクトの生成と破棄を行うクラス(場合によってはプールも行う)FactoryMethodみたいな感じです
class Factory
{
public:
	Factory(ObjectManager& objectManager);
	virtual ~Factory();

	/// <summary>
	/// 生成
	/// </summary>
	/// <param name="id"> 生成を指示したオブジェクトのID </param>
	/// <param name="pos"> 座標 </param>
	/// <param name="rot"> 回転 </param>
	/// <returns></returns>
	virtual ObjectID Create(ObjectID id, const Vector3& pos, const Vector3& rot = { 0.0f,0.0f,0.0f }) { return ObjectID{}; };

	/// <summary>
	/// 破棄用
	/// </summary>
	/// <param name="id"> 破棄するオブジェクトのID </param>
	virtual void Destroy(ObjectID id) {};

	/// <summary>
	/// IDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> FactoryID </returns>
	virtual const FactoryID GetID(void) const = 0;

	/// <summary>
	/// ロード完了したか？
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual const bool IsLoaded(void);

protected:

	/// <summary>
	/// ロード処理をする関数
	/// </summary>
	/// <param name="fileName"></param>
	void Load(const std::string& fileName);

	/// <summary>
	/// transformデータを読み込む(オーバーライドしなければ内部で読み飛ばす)
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="size"> データのバイト数 </param>
	virtual void LoadTrans(std::ifstream& file, std::uint32_t size);

	/// <summary>
	/// Colliderデータを読み込む(オーバーライドしなければ内部で読み飛ばす)
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="size"> データのバイト数 </param>
	virtual void LoadCollider(std::ifstream& file, std::uint32_t size);


	/// <summary>
	/// Modelデータを読み込む(オーバーライドしなければ内部で読み飛ばす)
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="size"> データのバイト数 </param>
	virtual void LoadModel(std::ifstream& file, std::uint32_t size);

	/// <summary>
	/// Spawnデータを読み込む(オーバーライドしなければ内部で読み飛ばす)
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="size"> データのバイト数 </param>
	virtual void LoadSpawn(std::ifstream& file, std::uint32_t size);

	/// <summary>
	/// Otherデータを読み込む(オーバーライドしなければ内部で読み飛ばす)
	/// </summary>
	/// <param name="file"> ファイルストリーム </param>
	/// <param name="size"> データのバイト数 </param>
	virtual void LoadOther(std::ifstream& file, std::uint32_t size);

	// オブジェクトマネージャー
	ObjectManager& objectManager_;
};

