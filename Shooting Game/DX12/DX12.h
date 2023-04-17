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

	// デバイスの初期化
	void CreateDvice(void);
	// コマンド系の初期化
	void CreateCommand(void);
	// コマンドキューの初期化
	void CreateCommandQueue(void);
};

