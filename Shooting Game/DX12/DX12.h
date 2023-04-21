#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <wrl/client.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;


class DX12
{
public:
	DX12(HWND hwnd);
	~DX12(void);
	// ���C�����[�v�ł̏���
	void Render(void);
	// ���_�ݒ�
	void CreateVertices(void);
	
private:
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;
	// �t���[����
	unsigned int frame;

	HWND hwnd_;

	// �f�o�C�X
	//ID3D12Device* dev_;
	ComPtr<ID3D12Device> dev_;
	// �t�@�N�g���[
	ComPtr<IDXGIFactory6> dxgiFactory_;
	// �X���b�v�`�F�[��
	ComPtr<IDXGISwapChain4> swapchain_;
	// �R�}���h�A���P�[�^�[
	ComPtr<ID3D12CommandAllocator> cmdAllcator_;
	// �R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// �R�}���h�L���[
	ComPtr<ID3D12CommandQueue> cmdQueue_;

	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
	vector<ComPtr<ID3D12Resource>> backBuffers_;

	// �t�F���X�ݒ�p�ϐ�
	ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_;

	// ���_�ݒ莞�p�̕ϐ�
	ComPtr<ID3D12Resource> vertBuff_;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// �p�C�v���C���X�e�[�g�p�ϐ�
	ComPtr<ID3D12PipelineState> pipelinestate_;
	// ���[�g�V�O�l�C�`���p�̕ϐ�
	ComPtr<ID3D12RootSignature> rootSig_;
	ComPtr<ID3DBlob> rootSigBlob_;

	// �V�F�[�_�[�I�u�W�F�N�g�ێ��p�ϐ�
	ComPtr<ID3DBlob> vsBlob_;
	ComPtr<ID3DBlob> psBlob_;
	// �G���[���ɕێ�����ϐ�
	ComPtr<ID3DBlob> errorBlob_;

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
	// �t�F���X�̐ݒ�
	void CreateFence(void);
	// �V�F�[�_�[�ǂݍ��ݎ��Ɏ��s�����Ƃ��̏���
	void FailedShader(HRESULT result);
	// �r���[�|�[�g�̐ݒ�
	void CreateView(void);
	// �V�U�[��`
	void CreateScissor(void);
};

