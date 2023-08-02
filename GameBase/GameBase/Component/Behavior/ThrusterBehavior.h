#pragma once
#include "Behavior.h"
#include "../ComponentHandle.h"
#include "../Render/EffectRender.h"

class ThrusterBehavior :
    public Behavior
{
public:
	ThrusterBehavior();

	/// <summary>
	/// スラスターエフェクトを再生する
	/// </summary>
	/// <param name=""></param>
	void Play(void);

	/// <summary>
	/// スラスターエフェクトを再生する（インターバル付き）
	/// </summary>
	/// <param name=""></param>
	void Play(float time);

	/// <summary>
	/// スラスターエフェクトを停止する
	/// </summary>
	/// <param name=""></param>
	void Stop(void);

	bool IsPlay(void) { return isPlay_; }

	ComponentID GetID(void) const override
	{
		return id_;
	}
	static constexpr ComponentID id_{ ComponentID::EffectBehavior };
private:
	void Update(BaseScene& scene, ObjectManager& objectManager, float delta, Controller& controller) final;
	void Begin(ObjectManager& objectManager) final;

	void UpdateThrusterInterval(float delta);

	// エフェクトの描画クラス
	ComponentHandle<EffectRender> effectRender_;

	// トランスフォーム
	ComponentHandle<Transform> transform_;

	float interval_;

	bool isInterval_;

	bool isPlay_;
};

