#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace std;

class DX12
{
public:
	DX12(void);
	~DX12(void);
private :
	// �f�o�C�X
	ID3D12Device* dev_;
	// �t�@�N�g���[
	IDXGIFactory6* dxgiFactory_;
	// �X���b�v�`�F�[��
	IDXGISwapChain4* swapchain_;
	// �R�}���h�A���P�[�^�[
	ID3D12CommandAllocator* cmdAllcator_;
	// �R�}���h���X�g
	ID3D12GraphicsCommandList* cmdList_;
	// �R�}���h�L���[
	ID3D12CommandQueue* cmdQueue;

	// �f�o�C�X�̏�����
	void CreateDvice(void);
	// �R�}���h�n�̏�����
	void CreateCommand(void);
	// �R�}���h�L���[�̏�����
	void CreateCommandQueue(void);
};

