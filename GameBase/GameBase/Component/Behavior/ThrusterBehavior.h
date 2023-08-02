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
	/// �X���X�^�[�G�t�F�N�g���Đ�����
	/// </summary>
	/// <param name=""></param>
	void Play(void);

	/// <summary>
	/// �X���X�^�[�G�t�F�N�g���Đ�����i�C���^�[�o���t���j
	/// </summary>
	/// <param name=""></param>
	void Play(float time);

	/// <summary>
	/// �X���X�^�[�G�t�F�N�g���~����
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

	// �G�t�F�N�g�̕`��N���X
	ComponentHandle<EffectRender> effectRender_;

	// �g�����X�t�H�[��
	ComponentHandle<Transform> transform_;

	float interval_;

	bool isInterval_;

	bool isPlay_;
};
