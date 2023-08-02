#pragma once
#include "../ComponentBase.h"
#include <memory>
#include <map>
#include "../ComponentHandle.h"
#include "../Render/ModelRender.h"

// アニメーションの再生状態を表すクラス(モデルのハンドルは分け合ってintで持ってる)
class AnimationState
{
public:
	virtual ~AnimationState() {}

	// 
	virtual AnimationState* Update(AnimationState* state, ObjectManager& objectManager,float delta) = 0;

	/// <summary>
	/// 再生時用初期化処理
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	virtual void Init(AnimationState* start, AnimationState* end) = 0;

	/// <summary>
	/// 再生終了しているか？
	/// </summary>
	/// <param name=""></param>
	virtual bool IsEnd(void) = 0;

	/// <summary>
	/// 再生時間をリスタートする
	/// </summary>
	/// <param name=""></param>
	void ReStart(void);

	/// <summary>
	/// アニメーションをアタッチする
	/// </summary>
	/// <param name=""></param>
	void Attach(void);

	/// <summary>
	/// アタッチしたインデックスを取得する
	/// </summary>
	/// <param name=""></param>
	int GetAttachIdx(void);

	/// <summary>
	/// アニメーションをデタッチする
	/// </summary>
	void Detach();

	/// <summary>
	/// モデルのハンドルをセット
	/// </summary>
	/// <param name="handle"></param>
	void SetHandle(int handle);

	/// <summary>
	/// モデルのハンドルを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	const int GetHandle(void) const;

	/// <summary>
	/// アニメーションのセットアップをする
	/// </summary>
	/// <param name="idx"></param>
	void SetUpAnim(int idx);

	/// <summary>
	/// アニメーションインデックスを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	int GetAnimIdx(void);

	
	/// <summary>
	/// ブレンドかどうか
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	virtual bool IsBlend(void) = 0;

	/// <summary>
	/// ループを行うかのフラグをセット
	/// </summary>
	/// <param name="flag"> フラグ </param>
	void SetLoopFlag(bool flag)
	{
		isLoop_ = flag;
	}

	/// <summary>
	/// 再生時間の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 再生時間 </returns>
	virtual float GetPlayTime(void) { return playTime_; }

	/// <summary>
	/// 超過分再生時間の取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 超過分再生時間 </returns>
	virtual float GetPlayTimeOver(void) { return playTimeOver_; }

	/// <summary>
	/// 総再生時間(最初から最後までの)を取得
	/// </summary>
	/// <param name=""></param>
	/// <returns> 総再生時間 </returns>
	virtual float GetTotalTime(void) { return totalTime_; }

	virtual Vector3 GetMovePosition(void) { return pos_; }

protected:

	// ハンドル
	int handle_{-1};

	// アタッチしたときのインデックス
	int attachIdx_{-1};

	// アニメーションのインデックス
	int animIdx_{-1};

	// ループするか？
	bool isLoop_{false};

	// 総再生時間
	float totalTime_{0.0f};

	// 再生時間
	float playTime_{0.0f};

	// 再生時間超過分
	float playTimeOver_{ 0.0f };

	Vector3 pos_;
	Vector3 prePos_;
	Vector3 nowPos_;

	int moveAnimFrameIndex_;


};

// 通常再生時の状態
class NormalState :
	public AnimationState
{
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	void Init(AnimationState* start, AnimationState* end) final;

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="state"></param>
	/// <param name="delta"></param>
	/// <returns></returns>
	AnimationState* Update(AnimationState* state, ObjectManager& objectManager, float delta) final;
	
	bool IsEnd(void) final;

	/// <summary>
	/// ブレンドかどうか
	/// </summary>
	/// <param name=""></param>
	/// <returns> ブレンドの時trueそうでないときfalse </returns>
	bool IsBlend(void)
	{
		return false;
	}
};


// ブレンド再生時の状態
class BlendState :
	public AnimationState
{
public:

	/// <summary>
	/// ブレンド時間のセット
	/// </summary>
	/// <param name="blendTime"> ブレンド時間 </param>
	void SetBlendTime(const float blendTime)
	{
		blendTime_ = blendTime;
	}
private:

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	void Init(AnimationState* start, AnimationState* end) final;

	/// <summary>
	/// ブレンドかどうか
	/// </summary>
	/// <param name=""></param>
	/// <returns> ブレンドの時trueそうでないときfalse </returns>
	bool IsBlend(void)
	{
		return true;
	}

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="state"></param>
	/// <param name="delta"></param>
	/// <returns></returns>
	AnimationState* Update(AnimationState* state, ObjectManager& objectManager, float delta) final;

	bool IsEnd(void) final;

	float GetTotalTime(void) final { return end_->GetTotalTime(); }
	float GetPlayTime(void) final { return end_->GetPlayTime(); }

	// ブレンド開始時の状態
	AnimationState* start_{nullptr};

	// ブレンド終了時の状態
	AnimationState* end_{nullptr};

	// ブレンドする時間
	float blendTime_{1.0f};
};


class Animator :
    public ComponentBase
{
public:
	ComponentID GetID(void) const override
	{
		return id_;
	}
	SetID(ComponentID::Animator, ComponentID::Animator)

	/// <summary>
	/// idxのアニメーションのへ移行する
	/// </summary>
	/// <param name="idx"></param>
	void SetNextAnimation(int idx, float blendTime = 1.0f);

	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;

	void Begin(ObjectManager& objectManager) final;

	/// <summary>
	/// アニメーションを追加する
	/// </summary>
	/// <param name="anitIdx"> モデルのアニメーションのインデックス </param>
	/// <param name="isLoop"> ループするか？ </param>
	void AddAnimation(int anitIdx, bool isLoop = true);

	/// <summary>
	/// アニメーションの状態を表すクラスを取得する
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	AnimationState* GetAnimState(void) { return nowState_; }
private:
	// 現在のステートクラス
	AnimationState* nowState_{nullptr};

	// ブレンド時に使用するステートクラス
	std::unique_ptr<BlendState> blendState_;

	// 各indexごとにアニメーション状態のクラスを保持
	std::map<int,std::unique_ptr<AnimationState>> animStates_;

	// 描画クラス
	ComponentHandle<ModelRender> render_;

};



