#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directXMath.h>
#include <wrl/client.h>
#include <DirectXTex.h>
#include <d3dx12.h>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"DirectXTex.lib")

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

struct TexRGBA
{
	unsigned char R, G, B, A;
};

struct Wave
{
	XMFLOAT2 dir;
	float amplitude;
	float waveLenght;
};

struct cbuff
{
	XMMATRIX worldMat;
	XMMATRIX viewMat;
	Wave waves[100];
};

// PMDヘッダ構造体
struct PMDHeader {
	float version;
	// モデルの名前
	char model_name[20];
	// モデルのコメント
	char comment[256];
};

// 頂点データ構造体
struct PMD_VERTEX
{
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT2 uv;
	uint16_t bone_no[2];
	uint8_t  weight;
	uint8_t  EdgeFlag;
	uint16_t dummy;
};


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
	// モデルの１頂点当たりのサイズ
	const size_t pmdvertexSize_ = 38;

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

	// テクスチャ用変数
	ComPtr<ID3D12Resource> texbuff_;
	// テクスチャ用のディスクリプタヒープ変数
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	ID3D12DescriptorHeap* texDescHeap = nullptr;

	// シェーダーオブジェクト保持用変数
	ComPtr<ID3DBlob> vsBlob_;
	ComPtr<ID3DBlob> psBlob_;
	// エラー時に保持する変数
	ComPtr<ID3DBlob> errorBlob_;

	// 定数バッファの変数
	ComPtr<ID3D12Resource> constBuff_;
	// 行列変数
	// マップ先を示すポインター
	XMMATRIX* mapMatrix_;
	cbuff* mapBuff_;
	ID3D12DescriptorHeap* basicDescHeap_ = nullptr;
	// お試しアニメーション
	float angle;
	XMMATRIX worldMat;
	XMMATRIX viewMat;
	XMMATRIX projMat;

	// ビューポート用変数
	D3D12_VIEWPORT viewport_ = {};
	// シザー矩形用変数
	D3D12_RECT scissorrect_ = {};
	// インデックスの用変数
	D3D12_INDEX_BUFFER_VIEW ibVIew_ = {};

	// モデルの読み込み用変数
	// シグネチャ
	char signature_[3] = {};
	PMDHeader pmdHeader = {};
	// 頂点数
	unsigned int vertNum_;
	vector<PMD_VERTEX> vertices_;
	// インデックス数
	unsigned int indicesNum;
	vector<unsigned short> indices_;
	ComPtr<ID3D12Resource> idxBuff_;

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
	// モデルのロード
	void LoadModel(void);
};

