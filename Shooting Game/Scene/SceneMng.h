#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
#include "../DX12/DX12.h"

#define lpSceneMng SceneMng::GetInstance()
#define Rand (lpSceneMng.GetRandom())

class SceneMng
{
public:
	// �ÓI�V���O���g��
	static SceneMng& GetInstance()
	{
		static SceneMng s_Instance;
		return s_Instance;
	}
	// ���s����
	void Run(void);

private:
	// �E�B���h�E�̃T�C�Y
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	// ����������
	bool SysInit(void);
	void Draw(float delta);
	bool InitFlag_;

	// directX12�̏�����
	unique_ptr<DX12> dx12;
	// �E�B���h�E�̐����p�ϐ�
	WNDCLASSEX w;

	// �f�o�b�O���C���[
	void EnableDebugLayer();
	void DebugOutputFormatString(const char* format, ...);

	SceneMng();
	~SceneMng();

};

