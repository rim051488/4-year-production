#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Render/EffectRender.h"
#include "../Transform/Transform.h"

enum class FactoryID;

// 処理の種類
enum class EffectBehaviorType
{
	SyncPos,		// 座標だけ同期する
	SyncRot,		// 回転だけ同期する
	SyncAll,		// 座標と回転両方同期する
	Non				// 何も同期しない時
};

// 一部エフェクトを再生終了と同時に消滅させたり制御するクラス
class EffectBehavior :
	public Behavior
{
public:
	EffectBehavior();

	/// <summary>
	/// ファクトリーIDのセット
	/// </summary>
	/// <param name="id"></param>
	void SetFactoryID(FactoryID id)
	{
		factoryID_ = id;
	}

	/// <summary>
	/// 動きの種類をセットする
	/// </summary>
	/// <param name="type"></param>
	void SetType(EffectBehaviorType type);

	/// <summary>
	/// 同期するオブジェクトのIDをセットする
	/// </summary>
	/// <param name="id"></param>
	void SetSyncID(ObjectID& id)
	{
		syncID_ = id;
	}

	/// <summary>
	/// オフセットをセットする
	/// </summary>
	/// <param name="offset"></param>
	void SetOffset(const Vector3 offset)
	{
		offset_ = offset;
	}

	ComponentID GetID(void) const override
	{
		return id_;
	}

	static constexpr ComponentID id_{ ComponentID::EffectBehavior };
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;
	void End(ObjectManager& objectManager) final;
	void Destory(ObjectManager& objManager) final;

	/// <summary>
	/// 何も同期しない時のアップデート
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateNonSync(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);
	
	/// <summary>
	/// 座標だけ同期する時のアップデート
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateSyncPos(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 回転だけ同期するアップデート
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateSyncRot(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 回転と座標両方同期する時のアップデート
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateSyncAll(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	// アップデート
	void(EffectBehavior::* update_)(BaseScene&, ObjectManager&, float, Controller&);

	// エフェクトの描画クラス
	ComponentHandle<EffectRender> effectRender_;

	// 同期相手のトランスフォーム
	ComponentHandle<Transform> syncTransform_;

	// 自分自身のトランスフォーム
	ComponentHandle<Transform> transform_;

	// 生成元のファクトリーID
	FactoryID factoryID_;

	// オフセット
	Vector3 offset_;

	// 同期するオブジェクトのID
	ObjectID syncID_;

	// 種類
	EffectBehaviorType type_;
};

