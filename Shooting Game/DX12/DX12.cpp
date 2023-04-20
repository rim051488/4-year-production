#include <vector>
#include <string>
#include "DX12.h"

DX12::DX12(HWND hwnd)
{
	hwnd_ = hwnd;
	// デバイスなどの初期化
	CreateDvice();
	CreateCommand();
	CreateCommandQueue();
	CreateSwapChain();
	CreateDescriptor();
}

DX12::~DX12(void)
{
}

void DX12::Render(void)
{
	// バックバッファのインデックスを取得
	auto bbIdx = swapchain_->GetCurrentBackBufferIndex();

	// バリアの処理
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	// 遷移
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// 指定なし
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バックバッファリソース
	BarrierDesc.Transition.pResource = backBuffers_[bbIdx].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// 直前はPRESENT状態
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 今からレンダーターゲット状態
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// バリアの指定実行
	cmdList_->ResourceBarrier(1, &BarrierDesc);

	// レンダーターゲットを指定
	auto rtvH = rtvHeaps_->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += static_cast<ULONG_PTR>(bbIdx * dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	cmdList_->OMSetRenderTargets(1, &rtvH, false, nullptr);

	// 画面クリア
	float clearColor[] = { 1.0f,1.0f,0.0f,1.0f };// 黄色
	cmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);

	// 前後だけを入れ替える
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	cmdList_->ResourceBarrier(1, &BarrierDesc);

	// 命令のクローズ
	cmdList_->Close();

	// コマンドリストの実行
	ID3D12CommandList* cmdlists = cmdList_.Get();
	cmdQueue_->ExecuteCommandLists(1, &cmdlists);

	// 待ち
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// キューをクリア
	cmdAllcator_->Reset();
	// コマンドリストをためる準備
	cmdList_->Reset(cmdAllcator_.Get(), nullptr);

	// フリップ
	swapchain_->Present(1, 0);
}

void DX12::CreateDvice(void)
{
	dev_ = nullptr;
	dxgiFactory_ = nullptr;
	// デバイスの初期化をする前にグラボが何かを調べる
	if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
	{
		if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
		{
			return;
		}
	};

	// アダプターの列挙用
	vector<IDXGIAdapter*> adapters;

	// ここに特定の名前を持つアダプターオブジェクトが入る
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0; dxgiFactory_->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		// アダプターの説明オブジェクト取得
		adpt->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		// 探したいアダプターの名前を確認
		if (strDesc.contains(L"NVIDIA") || strDesc.contains(L"AMD"))
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// デバイスの初期化
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	D3D_FEATURE_LEVEL featurelevel;
	for (auto level : levels)
	{
		// 生成可能かどうかを調べる
		if (D3D12CreateDevice(tmpAdapter, level, IID_PPV_ARGS(dev_.ReleaseAndGetAddressOf())) == S_OK)
		{
			featurelevel = level;
			break;
		}
	}
	for (auto adpt : adapters)
	{
		adpt->Release();
	}
}

void DX12::CreateCommand(void)
{
	cmdAllcator_ = nullptr;
	cmdList_ = nullptr;
	// コマンドアロケーターの初期化
	auto result = dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllcator_.ReleaseAndGetAddressOf()));
	// コマンドリストの初期化
	result = dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllcator_.Get(), nullptr, IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf()));
}

void DX12::CreateSwapChain(void)
{
	swapchain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	// 画像解像度幅
	swapchainDesc.Width = window_width;
	// 画像解像度高
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ステレオ表示フラグ
	swapchainDesc.Stereo = false;
	// マルチサンプルの指定(Countは基本は１)
	swapchainDesc.SampleDesc.Count = 1;
	// マルチサンプルの指定(Qualityは基本は０)
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	// ダブルバッファなので２
	swapchainDesc.BufferCount = 2;

	// バックバッファは伸び縮み可能
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	// フリップ後は速やかに破棄
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// 指定なし
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// ウィンドウ⇔フルスクリーンの切り替え可能
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	auto result = dxgiFactory_->CreateSwapChainForHwnd(
		cmdQueue_.Get(),
		hwnd_,
		&swapchainDesc,
		nullptr,
		nullptr,
		(IDXGISwapChain1**)swapchain_.GetAddressOf());		
}

void DX12::CreateDescriptor(void)
{
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	DXGI_SWAP_CHAIN_DESC swcDesc = {};

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			// レンダーターゲットビューなのでRTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;							// 表裏の２つ
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		// 指定なし

	rtvHeaps_ = nullptr;

	auto result = dev_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeaps_.ReleaseAndGetAddressOf()));

	// スワップチェーンとメモリのひも付け
	result = swapchain_->GetDesc(&swcDesc);
	backBuffers_.resize(swcDesc.BufferCount);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps_->GetCPUDescriptorHandleForHeapStart();

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = swapchain_->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(backBuffers_[idx].ReleaseAndGetAddressOf()));
		// ０番目以外のディスクリプタを取得するために
		dev_->CreateRenderTargetView(
			backBuffers_[idx].Get(),
			nullptr,
			handle);
		handle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	CreateFence();
}

void DX12::CreateCommandQueue(void)
{
	cmdQueue_ = nullptr;
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	// タイムアウト無し
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// アダプターを１つしか使わない時は０でよい
	cmdQueueDesc.NodeMask = 0;
	// プライオリティは特に指定なし
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// コマンドリストと合わせる
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// キューの生成
	auto result = dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(cmdQueue_.ReleaseAndGetAddressOf()));
}

void DX12::CreateFence(void)
{
	fence_ = nullptr;
	fenceVal_ = 0;

	auto result = dev_->CreateFence(
		fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence_.ReleaseAndGetAddressOf()));
}

void DX12::FailedShader(HRESULT result)
{
	if (FAILED(result))
	{
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
		{
			OutputDebugStringA("ファイルが見当たりません");
			return;
		}
	}
	else
	{
		string errorstr;
		errorstr.resize(errorBlob_->GetBufferSize());
		copy_n((char*)errorBlob_->GetBufferPointer(),
			errorBlob_->GetBufferSize(),
			errorstr.begin());
		errorstr += "\n";
		OutputDebugStringA(errorstr.c_str());
	}
}

void DX12::CreateVertices(void)
{
	// 頂点バッファーの生成
	XMFLOAT3 vertices[] =
	{
		{-1.0f,-1.0f,0.0f}, // 左下
		{-1.0f,1.0f,0.0f}, // 左上
		{1.0f,-1.0f,0.0f}, // 右下
	};

	// 頂点ヒープの設定
	D3D12_HEAP_PROPERTIES heapprop = {};
	// ヒープの種類
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPUのページング設定
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// メモリプールがどこかを示す
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// リソース設定
	D3D12_RESOURCE_DESC resdesc = {};
	// バッファで使うのでBUFFERを指定
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// 幅で全部まかなうのでsizeof(全頂点)とする
	resdesc.Width = sizeof(vertices);
	// 幅で表現しているので1とする
	resdesc.Height = 1;
	// １でよい
	resdesc.DepthOrArraySize = 1;
	// １でよい
	resdesc.MipLevels = 1;
	// 画像ではないのでUNKNOWNでよい
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	// SampleDesc.Count = 1とする
	resdesc.SampleDesc.Count = 1;
	// D3D12_TEXTURE_LAYOUT_ROW_MAJORとする
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	// NONEでよい
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	auto result = dev_->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff_.ReleaseAndGetAddressOf()));

	// 頂点情報のコピー
	XMFLOAT3* vertMap = nullptr;

	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);

	copy(begin(vertices), end(vertices), vertMap);

	vertBuff_->Unmap(0, nullptr);

	D3D12_VERTEX_BUFFER_VIEW vbView = {};
	// バッファーの仮想アドレス
	vbView.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// 全バイト数
	vbView.SizeInBytes = sizeof(vertices);
	// １頂点あたりのバイト数
	vbView.StrideInBytes = sizeof(vertices[0]);
	// 頂点シェーダーの設定
	result = D3DCompileFromFile(
		L"Shader/VS.hlsl",														// シェーダー名
		nullptr,																// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,										// インクルードはデフォルト
		"BasicVS", "vs_5_0",													// 関数はBaasicVS、対象シェーダーはvs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,						// デバッグ用及び最適化なし
		0,
		vsBlob_.ReleaseAndGetAddressOf(), errorBlob_.ReleaseAndGetAddressOf());	// コンパイル成功時はvsBlob_に、失敗時はerrorBlob_に格納
	// ピクセルシェーダーの設定
	result = D3DCompileFromFile(
		L"Shader/PS.hlsl",														// シェーダー名
		nullptr,																// defineはなし
		D3D_COMPILE_STANDARD_FILE_INCLUDE,										// インクルードはデフォルト
		"BasicPS", "ps_5_0",													// 関数はBaasicVS、対象シェーダーはvs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,						// デバッグ用及び最適化なし
		0,
		psBlob_.ReleaseAndGetAddressOf(), errorBlob_.ReleaseAndGetAddressOf());	// コンパイル成功時はpsBlob_に、失敗時はerrorBlob_に格納
	//FailedShader(result);
	// 頂点レイアウトの設定
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		}
	};
	// グラフィックスパイプラインステートの作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	// あとで設定する
	gpipeline.pRootSignature = nullptr;
	// シェーダーのセット
	// 頂点シェーダーの設定
	gpipeline.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob_->GetBufferSize();
	// ピクセルシェーダーの設定
	gpipeline.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob_->GetBufferSize();
	// サンプルマスクとラスタライザーステートの設定
	// デフォルトのサンプルマスクを表す定数(0xffffffff)
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// アンチエイリアシングは使わないのでfalseにする
	gpipeline.RasterizerState.MultisampleEnable = false;
	// カリングしない
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// 中身を塗りつぶす
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// 深度方向のクリッピングは有効に
	gpipeline.RasterizerState.DepthClipEnable = true;
	// ブレンドステートの設定
	// 
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	// 
	gpipeline.BlendState.IndependentBlendEnable = false;
	// 
	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.LogicOpEnable = false;
}
