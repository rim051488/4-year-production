#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <wrl/client.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace DirectX;
using namespace Microsoft::WRL;

struct MyVertex
{
	float x, y, z;
};

class DX12
{
public:
	static constexpr int RTV_NUM = 2;
	DX12(HWND hwnd, int window_widht, int window_height);
	~DX12();
	HRESULT Render();
private:
	HWND	window_hwnd_;
	int		window_width_;
	int		window_height_;

	HANDLE	fence_event_;
	// �f�o�C�X
	ComPtr<ID3D12Device> device_;
	// �R�}���h�L���[(�R�}���h����)
	ComPtr<ID3D12CommandQueue> command_queue_;
	// �R�}���h�A���P�[�^�[
	ComPtr<ID3D12CommandAllocator> command_allocator_;
	// �R�}���h���X�g
	ComPtr<ID3D12GraphicsCommandList> command_list_;
	// �X���b�v�`�F�C��(�o�b�N�o�b�t�@�Ȃǂ̏���)
	ComPtr<IDXGISwapChain3> swap_chain_;
	// �L���[�t�F���X(�L���[�𓊂��ď����҂�)
	ComPtr<ID3D12Fence> queue_fence_;
	// �t�@�N�g���[
	ComPtr<IDXGIFactory3> factory_;
	// �f�B�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descriptor_heap_;
	// ���C���ƃo�b�N�̃o�b�t�@
	ComPtr<ID3D12Resource> render_target_[2];
	// �n���h��
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle_[RTV_NUM];
	// �V�F�[�_�[�̐ݒ�
	struct ShaderObject
	{
		void* binaryptr;
		int size;
	};
	ComPtr<ID3D12RootSignature> root_signature_;
	ComPtr<ID3D12PipelineState> pipeline_state_;
	ShaderObject vertex_shader;
	ShaderObject pixel_shader;
	ComPtr<ID3D12Resource> vertex_buffer_;
	D3D12_VERTEX_BUFFER_VIEW buffer_pos_;
	ComPtr<IDXGIAdapter> adapter_;

	// �t�@�N�g���[���쐬
	void CreateFactory(HRESULT hr, UINT flagsDXGI);
	// �f�o�C�X���쐬
	void CreateDevice(HRESULT hr);
	// �R�}���h�̍쐬
	void CreateCommand(HRESULT hr);
	// �����_�[�^�[�Q�b�g�̐���
	void CreateRenderTargetView(HRESULT hr);

	// �o���A�̐ݒ�
	void SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	// �L���[�ƃR�}���h�̏�����҂�
	void WaitForCommandQueue(ID3D12CommandQueue* pCommandQueue);
	// �V�F�[�_�[�Ȃǂ̃Z�b�g�A�b�v
	bool resourceSetup();
};

