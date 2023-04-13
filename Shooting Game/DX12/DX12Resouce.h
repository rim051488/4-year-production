#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <d3d12shader.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <wrl/client.h>

using namespace DirectX;
using namespace Microsoft::WRL;

class DX12Resouce
{
public:
	static constexpr int RTV_NUM = 2;
	DX12Resouce(HWND hwnd, int window_widht, int window_height);
	~DX12Resouce();
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
	// �R�}���h���P�[�^�[
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

	// �o���A�̐ݒ�
	void SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	// �L���[�ƃR�}���h�̏�����҂�
	void WaitForCommandQueue(ID3D12CommandQueue* pCommandQueue);
};

