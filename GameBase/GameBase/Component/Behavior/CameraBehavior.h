#pragma once
#include "Behavior.h"
#include "../Transform/Transform.h"
#include "../ComponentHandle.h"

class CameraBehavior :
    public Behavior
{
public:

	enum class Type
	{
		Sync,		// オブジェクトと同期する
		Non,		// 特に何もしない
		Change		// 切り替え
	};

	ComponentID GetID(void) const final
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::CameraBehavior };

	/// <summary>
	/// カメラのシェイクを開始する
	/// </summary>
	/// <param name="shakeTime"> シェイクする時間 </param>
	void StartShake(float shakeTime);

	/// <summary>
	/// 同期するオブジェクトのIDをセットする
	/// </summary>
	/// <param name="id"></param>
	void SetSyncID(ObjectID& id)
	{
		syncID_ = id;
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="speed"></param>
	void SetSpeed(float speed);

	void SetType(Type type);
private:

	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;

	// 更新処理
	void (CameraBehavior::* update_)(BaseScene&, ObjectManager&, float, Controller&);

	/// <summary>
	/// オブジェクトと同期する時の更新処理
	/// </summary>
	/// <param name="scene">  </param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateSync(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);

	/// <summary>
	/// 何もしないとき用の更新処理
	/// </summary>
	/// <param name="scene"></param>
	/// <param name="objectManager"></param>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateNon(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller);


	/// <summary>
	/// カメラシェイク処理
	/// </summary>
	/// <param name="delta"></param>
	void Shake(float delta);

	/// <summary>
	/// カメラオフセット
	/// </summary>
	/// <param name="delta"></param>
	void CameraOffset(float delta);

	/// <summary>
	/// 経過時間
	/// </summary>
	float stepTime_;

	/// <summary>
	/// 自身のトランスフォーム
	/// </summary>
	ComponentHandle<Transform> transform_;

	/// <summary>
	/// 同期先のオブジェクトID
	/// </summary>
	ObjectID syncID_;

	/// <summary>
	/// シェイク前の座標
	/// </summary>
	Vector3 defPos_;

	/// <summary>
	/// カメラをシェイクしているか
	/// </summary>
	bool isShake_;

	/// <summary>
	/// カメラをシェイクする時間
	/// </summary>
	float shakeTime_;

	/// <summary>
	/// カメラ操作スピード
	/// </summary>
	float speed_;

	/// <summary>
	/// ダッシュ中のカメラ座標オフセット
	/// </summary>
	float offset_;

	/// <summary>
	/// オフセット用カウンタ
	/// </summary>
	float cnt_;

	/// <summary>
	/// オフセットの最大値
	/// </summary>
	float offsetMax_;

	Type nowType_;
};

