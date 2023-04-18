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
	DX12(HWND hwnd);
	~DX12(void);
	// ���C�����[�v�ł̏���
	void Render(void);

private :
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	HWND hwnd_;

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

	ID3D12DescriptorHeap* rtvHeaps_;
	vector<ID3D12Resource*> backBuffers_;

	// �t�F���X�ݒ�p�ϐ�
	ID3D12Fence* fence;
	UINT64 fenceVal;

	// �f�o�C�X�̏�����
	void CreateDvice(void);
	// �R�}���h�n�̏�����
	void CreateCommand(void);
	// �R�}���h�L���[�̏�����
	void CreateCommandQueue(void);
	// �X���b�v�`�F�[���̏�����
	void CreateSwapChain(void);
	// �f�B�X�N���v�^�q�[�v�̏�����
	void CreateDescriptor(void);
	// �X���b�v�`�F�[���ƃ������̂Ђ��t��
	void SwapChainMemory(void);
	// �t�F���X�̐ݒ�
	void CreateFence(void);
};

