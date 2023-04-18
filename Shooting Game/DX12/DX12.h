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
	// メインループでの処理
	void Render(void);

private :
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	HWND hwnd_;

	// デバイス
	ID3D12Device* dev_;
	// ファクトリー
	IDXGIFactory6* dxgiFactory_;
	// スワップチェーン
	IDXGISwapChain4* swapchain_;
	// コマンドアロケーター
	ID3D12CommandAllocator* cmdAllcator_;
	// コマンドリスト
	ID3D12GraphicsCommandList* cmdList_;
	// コマンドキュー
	ID3D12CommandQueue* cmdQueue;

	ID3D12DescriptorHeap* rtvHeaps_;
	vector<ID3D12Resource*> backBuffers_;

	// フェンス設定用変数
	ID3D12Fence* fence;
	UINT64 fenceVal;

	// デバイスの初期化
	void CreateDvice(void);
	// コマンド系の初期化
	void CreateCommand(void);
	// コマンドキューの初期化
	void CreateCommandQueue(void);
	// スワップチェーンの初期化
	void CreateSwapChain(void);
	// ディスクリプタヒープの初期化
	void CreateDescriptor(void);
	// スワップチェーンとメモリのひも付け
	void SwapChainMemory(void);
	// フェンスの設定
	void CreateFence(void);
};

