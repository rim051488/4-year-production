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
	// デバイス
	ComPtr<ID3D12Device> device_;
	// コマンドキュー(コマンド処理)
	ComPtr<ID3D12CommandQueue> command_queue_;
	// コマンドアロケーター
	ComPtr<ID3D12CommandAllocator> command_allocator_;
	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> command_list_;
	// スワップチェイン(バックバッファなどの処理)
	ComPtr<IDXGISwapChain3> swap_chain_;
	// キューフェンス(キューを投げて処理待ち)
	ComPtr<ID3D12Fence> queue_fence_;
	// ファクトリー
	ComPtr<IDXGIFactory3> factory_;
	// ディスクリプタヒープ
	ComPtr<ID3D12DescriptorHeap> descriptor_heap_;
	// メインとバックのバッファ
	ComPtr<ID3D12Resource> render_target_[2];
	// ハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle_[RTV_NUM];
	// シェーダーの設定
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

	// ファクトリーを作成
	void CreateFactory(HRESULT hr, UINT flagsDXGI);
	// デバイスを作成
	void CreateDevice(HRESULT hr);
	// コマンドの作成
	void CreateCommand(HRESULT hr);
	// レンダーターゲットの生成
	void CreateRenderTargetView(HRESULT hr);

	// バリアの設定
	void SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);
	// キューとコマンドの処理を待つ
	void WaitForCommandQueue(ID3D12CommandQueue* pCommandQueue);
	// シェーダーなどのセットアップ
	bool resourceSetup();
};

