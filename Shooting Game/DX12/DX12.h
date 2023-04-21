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
	// メインループでの処理
	void Render(void);
	// 頂点設定
	void CreateVertices(void);
	
private:
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;
	// フレーム数
	unsigned int frame;

	HWND hwnd_;

	// デバイス
	//ID3D12Device* dev_;
	ComPtr<ID3D12Device> dev_;
	// ファクトリー
	ComPtr<IDXGIFactory6> dxgiFactory_;
	// スワップチェーン
	ComPtr<IDXGISwapChain4> swapchain_;
	// コマンドアロケーター
	ComPtr<ID3D12CommandAllocator> cmdAllcator_;
	// コマンドリスト
	ComPtr<ID3D12GraphicsCommandList> cmdList_;
	// コマンドキュー
	ComPtr<ID3D12CommandQueue> cmdQueue_;

	ComPtr<ID3D12DescriptorHeap> rtvHeaps_;
	vector<ComPtr<ID3D12Resource>> backBuffers_;

	// フェンス設定用変数
	ComPtr<ID3D12Fence> fence_;
	UINT64 fenceVal_;

	// 頂点設定時用の変数
	ComPtr<ID3D12Resource> vertBuff_;
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};

	// パイプラインステート用変数
	ComPtr<ID3D12PipelineState> pipelinestate_;
	// ルートシグネイチャ用の変数
	ComPtr<ID3D12RootSignature> rootSig_;
	ComPtr<ID3DBlob> rootSigBlob_;

	// シェーダーオブジェクト保持用変数
	ComPtr<ID3DBlob> vsBlob_;
	ComPtr<ID3DBlob> psBlob_;
	// エラー時に保持する変数
	ComPtr<ID3DBlob> errorBlob_;

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
	// フェンスの設定
	void CreateFence(void);
	// シェーダー読み込み時に失敗したときの処理
	void FailedShader(HRESULT result);
	// ビューポートの設定
	void CreateView(void);
	// シザー矩形
	void CreateScissor(void);
};

