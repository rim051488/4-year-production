#pragma once
#include "BaseScene.h"

class ObjectManager;


class TitleScene :
	public BaseScene
{
public:
	TitleScene();
	~TitleScene();
	// ���C�g�J�����̒����_
	static constexpr Vector3 camTar{ 0.0f, 0.0f, -0 };
	// ���C�g�J�����̐��ˉe�̕\���͈�
	static constexpr float offsetOrtho = 1000.0f;
	// ���C�g�J�����̎�O�̋����Ɖ��̋���
	static constexpr float offsetNear = 0.001f;
	static constexpr float offsetFar = 2000.0f;
	// �L���[�u�}�b�v�̃T�C�Y
	static constexpr int cubeSize = 1024;
	// �L���[�u�}�b�v�̐�
	static constexpr int cubeNum = 6;


private:
	const SceneID GetID(void) const
	{
		return SceneID::Title;
	}
	SceneUptr MakeGameFunc(SceneUptr own);
	void Update(float delta, Controller& controller) final;
	void DrawScene(void) final;
	bool IsLoaded(void) final;
	
	/// <summary>
	/// �ʏ펞�̍X�V
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateNon(float delta, Controller& controller);

	/// <summary>
	/// ���S�������Ƃ��̍X�V
	/// </summary>
	/// <param name="delta"></param>
	/// <param name="controller"></param>
	void UpdateLogoOff(float delta, Controller& controller);

	/// <summary>
	/// ���S�������Ƃ��̕`��
	/// </summary>
	/// <param name=""></param>
	void DrawLogoOff(void);

	/// <summary>
	/// �ʏ펞�̕`��
	/// </summary>
	/// <param name=""></param>
	void DrawNon(void);

	/// <summary>
	/// �w�i�I�u�W�F�N�g�𐶐�����
	/// </summary>
	/// <param name=""></param>
	void CreateBackGround(void);

	/// <summary>
	/// ���[�h�������̏���
	/// </summary>
	/// <param name="controller"> �R���g���[���[ </param>
	void Loaded(Controller& controller);

	///// <summary>
	///// �V���h�E�}�b�v�̍쐬
	///// </summary>
	///// <param name=""></param>
	//void SetupShadowMap(void);

	/// <summary> ����`�悷��ʂ̐� </summary>
	/// <param name=""></param>
	void cubeDraw(void);

	///// <summary>
	///// �g�V�F�[�_�[�̎���
	///// </summary>
	///// <param name="model">���f��</param>
	///// <param name="vs">���_�V�F�[�_�[</param>
	///// <param name="ps">�s�N�Z���V�F�[�_�[</param>
	///// <param name="dynamicCube">�L���[�u���</param>
	///// <param name="cube"></param>
	//void DrawModelForMyShader(int model, int vs, int ps, int dynamicCube, int cube);

	// �X�V����
	void (TitleScene::* update_)(float, Controller&);

	// �`�揈��
	void (TitleScene::* draw_)(void);

	// �o�ߎ���
	float stepTime_;

	int skyDomeModel_;
	// �L���[�u�}�b�v�e�N�X�`��
	int dynamicCubeTex_;
	int cubeTex_;

};

