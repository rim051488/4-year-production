#pragma once
#include <DxLib.h>
#include <memory>
#include <unordered_map>
#include <forward_list>
#include <map>
#include <mutex>
#include "../Component/ComponentHandle.h"
#include "../Factory/FactoryID.h"
#include "ObjectID.h"
#include "../Component/ComponentID.h"
#include "../Common/Vector3.h"
#include "../Component/Info/ObjectAttribute.h"

class BaseScene;
class ObjectManager;
class ObjectID;
class Factory;
class ComponentPool;
class Controller;

class ThreadPool;

// オブジェクトを管理するクラス
class ObjectManager
{
	using ComponentUptr = std::unique_ptr<ComponentBase>;
	using ComponentMap = std::unordered_map<size_t, ComponentUptr >;
public:
	ObjectManager(size_t objectMax);
	~ObjectManager();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="delta"> デルタタイム </param>
	/// <param name="scene"> シーン </param>
	void Update(float delta, Controller& controller, BaseScene& scene);

	/// <summary>
	/// シャドウマップを考慮して描画する
	/// </summary>
	/// <param name="shadowMap"></param>
	/// <param name="buff"></param>
	void ShadowDraw(int shadowMap = -1,int buff = -1,int cubeTex = -1);

	/// <summary>
	/// 深度テクスチャを作成
	/// </summary>
	/// <param name="ps">深度作成用のピクセルシェーダ</param>
	void SetupDepthTex(int ps = -1,int buffer = -1);

	/// <summary>
	/// 指定オブジェクトIDのコンポーネントを取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	template<CompC T>
	ComponentHandle<T> GetComponent(const ObjectID& id);

	/// <summary>
	/// オブジェクトがアクティブかを返す
	/// </summary>
	/// <param name="id"> オブジェクトID </param>
	/// <returns></returns>
	bool IsActive(const ObjectID& id);

	/// <summary>
	/// IDを生成
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectID MakeObjectID(void);

	/// <summary>
	/// コンポーネントの追加
	/// </summary>
	/// <param name="component"> 追加するコンポーネント </param>
	/// <param name="id"> ID </param>
	/// <returns></returns>
	template<CompC T>
	bool AddComponent(std::unique_ptr<T> component, ObjectID& id);

	/// <summary>
	/// コンポーネントの削除
	/// </summary>
	/// <param name="id"> 削除するコンポーネントを持つオブジェクトID </param>
	/// <returns> 削除したコンポーネント </returns>
	template<CompC T>
	std::unique_ptr<T> RemoveComponent(ObjectID& id);

	/// <summary>
	/// 指定したコンポーネントを持っているか？
	/// </summary>
	/// <param name="id"> 調べるオブジェクトのID </param>
	/// <returns></returns>
	template<CompC T>
	bool HaveComponent(const ObjectID& id);

	/// <summary>
	/// すべてのコンポーネントを削除する
	/// </summary>
	/// <param name="id"> 対象のオブジェクトのID </param>
	void RemoveAllComponent(const ObjectID& id);

	/// <summary>
	/// 開始処理
	/// </summary>
	/// <param name=""></param>
	void Begin(void);

	/// <summary>
	/// 指定のIDのオブジェクトを開始処理する
	/// </summary>
	/// <param name="id"></param>
	void Begin(ObjectID& id);

	/// <summary>
	/// 指定のオブジェクトの終了処理をする
	/// </summary>
	/// <param name="id"></param>
	void End(ObjectID& id);

	/// <summary>
	/// 生成用クラスの登録
	/// </summary>
	/// <param name="factory"></param>
	void AddFactory(std::unique_ptr<Factory>&& factory);

	/// <summary>
	/// 生成用クラスの取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	Factory& GetFactory(const FactoryID id);

	/// <summary>
	/// ファクトリーIDのオブジェクトを指定座標に生成
	/// </summary>
	/// <param name="id"> 使用するfactoryのID </param>
	/// <param name="pos"> 生成する座標 </param>
	ObjectID CreateFromFactory(const FactoryID id, const ObjectID& objID, const Vector3 &pos, const Vector3& rot = { 0.0f, 0.0f, 0.0f});

	/// <summary>
	/// 生成用クラスが存在するか
	/// </summary>
	/// <param name="id"> 生成用クラスのID </param>
	/// <returns></returns>
	bool HaveFactory(const FactoryID id);

	/// <summary>
	/// ロード完了してるか？
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const bool IsLoaded(void);

	/// <summary>
	/// プレイヤーのID
	/// </summary>
	/// <param name="id"></param>
	void SetPlayerID(ObjectID id)
	{
		playerID_ = id;
	}

	/// <summary>
	/// プレイヤーIDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectID& GetPlayerID(void)
	{
		return playerID_;
	}

	/// <summary>
	/// プレイヤー攻撃のID
	/// </summary>
	/// <param name="id"></param>
	void SetPlayerAttackID(ObjectID id)
	{
		playerAttackID_ = id;
	}

	/// <summary>
	/// プレイヤー攻撃IDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectID& GetPlayerAttackID(void)
	{
		return playerAttackID_;
	}

	/// <summary>
	/// エネミーのID
	/// </summary>
	/// <param name="id"></param>
	void SetEnemyID(ObjectID id)
	{
		enemyID_ = id;
	}

	/// <summary>
	/// エネミーIDの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectID& GetEnemyID(void)
	{
		return enemyID_;
	}

	ComponentPool& GetPool(void);

	/// <summary>
	/// ヒットストップ風の演出を開始する
	/// </summary>
	/// <param name="stopTime"></param>
	void StartHitStop(const float stopTime);

	/// <summary>
	/// アトリビュートに一致するオブジェクトを探す
	/// </summary>
	/// <param name="atr"> 探したいオブジェクトのアトリビュート </param>
	std::pair<bool, ObjectID> Find(ObjectAttribute atr);


	/// <summary>
	/// カメラ制御オブジェクトのIDをセットする
	/// </summary>
	/// <param name="id"></param>
	void SetCameraID(ObjectID& id)
	{
		cameraID_ = id;
	}

	/// <summary>
	/// カメラ制御オブジェクトのIDを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ObjectID& GetCameraID(void)&
	{
		return cameraID_;
	}

	ObjectID& GetStageID(void)&
	{
		return stageID_;
	}

	void SetStageID(ObjectID& id)
	{
		stageID_ = id;
	}
private:

	// プレイヤーのID
	ObjectID playerID_;

	// プレイヤー攻撃のID
	ObjectID playerAttackID_;

	// エネミーのID
	ObjectID enemyID_;

	// カメラ制御用
	ObjectID cameraID_;

	// コンポーネントをマップで持つ
	std::map<ComponentID, ComponentMap > componentMap_;

	// 生成用クラス
	std::map<FactoryID, std::unique_ptr<Factory>> factoryMap_;

	// プール用クラス
	std::unique_ptr<ComponentPool> pool_;

	// ヒットストップの演出をしているか
	bool isHitStop_;

	// ヒットストップ演出の時間
	float hitStopTime_;

	// ミューテックス
	std::mutex mutex_;

	// ステージのID
	ObjectID stageID_;
};

template<CompC T>
inline ComponentHandle<T> ObjectManager::GetComponent(const ObjectID& id)
{
	std::lock_guard<std::mutex> lock{ mutex_ };
	if (componentMap_.contains(T::baseID_))
	{
		if (componentMap_[T::baseID_].contains(*id))
		{
			auto r = (static_cast<unsigned int>(T::id_) & static_cast<unsigned int>(componentMap_[T::baseID_][*id]->GetID()));
			if (r == static_cast<unsigned int>(T::id_))
			{
				return ComponentHandle<T>{ id,static_cast<T*>(componentMap_[T::baseID_][*id].get()), this};
			}
		}
	}
	return ComponentHandle<T>{id,nullptr,this};
}

template<CompC T>
inline bool ObjectManager::AddComponent(std::unique_ptr<T> component, ObjectID& id)
{
	std::lock_guard<std::mutex> lock{ mutex_ };
	if (componentMap_[T::baseID_].contains(*id))
	{
		return false;
	}
	component->SetOwnerID(id);
	componentMap_[T::baseID_][*id] = std::move(component);
	return true;
}


template<CompC T>
inline std::unique_ptr<T> ObjectManager::RemoveComponent(ObjectID& id)
{
	std::lock_guard<std::mutex> lock{ mutex_ };
	if (componentMap_.contains(T::baseID_))
	{
		if (componentMap_[T::baseID_].contains(*id))
		{
			auto comp = std::move(componentMap_[T::baseID_][*id]);
			componentMap_[T::baseID_].erase(*id);
			auto r = static_cast<unsigned int>(T::id_) & static_cast<unsigned int>(comp->GetID());
			if (r == static_cast<unsigned int>(T::id_))
			{
				auto ptr = comp.get();
				comp.release();
				return std::unique_ptr<T>{static_cast<T*>(ptr)};
			}
		}
	}
	return std::unique_ptr<T>{nullptr};
}

template<CompC T>
inline bool ObjectManager::HaveComponent(const ObjectID& id)
{
	return componentMap_.contains(T::baseID_);
}

