#include <vector>
#include <string>
#include "DX12.h"

DX12::DX12(HWND hwnd)
{
	hwnd_ = hwnd;
	frame = 0;
	angle = 0;
	// デバイスなどの初期化
	CreateDvice();
	CreateCommand();
	CreateCommandQueue();
	CreateSwapChain();
	CreateDescriptor();
}

DX12::~DX12(void)
{
	//// テクスチャディスクリプタの初期化
	//texDescHeap->Release();
	// バッファの初期化
	basicDescHeap_->Release();
}

void DX12::Render(void)
{
	//angle += 0.01f;
	//worldMat = XMMatrixRotationY(angle);
	//*mapMatrix_ = worldMat * viewMat * projMat;

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
	float r, g, b;
	r = 1.0f/*(float)(0xff & frame >> 16) / 255.0f*/;
	g = 1.0f/*(float)(0xff & frame >> 8) / 255.0f*/;
	b = 1.0f/*(float)(0xff & frame >> 0) / 255.0f*/;
	float clearColor[] = { r,g,b,1.0f };
	cmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	++frame;
	cmdList_->RSSetViewports(1, &viewport_);
	cmdList_->RSSetScissorRects(1, &scissorrect_);
	cmdList_->SetPipelineState(pipelinestate_.Get());
	cmdList_->SetGraphicsRootSignature(rootSig_.Get());
	cmdList_->SetDescriptorHeaps(1,  &basicDescHeap_);
	cmdList_->SetGraphicsRootDescriptorTable(
		0,// ルートパラメーターインデックス
		basicDescHeap_->GetGPUDescriptorHandleForHeapStart());// ヒープアドレス
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	cmdList_->IASetIndexBuffer(&ibVIew_);
	// インデックスを使用する場合
	//cmdList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
	cmdList_->DrawInstanced(indicesNum, 1, 0, 0);

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

	// SRGBレンダーターゲットビュー設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	// ガンマ補正あり
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = swapchain_->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(backBuffers_[idx].ReleaseAndGetAddressOf()));
		// ０番目以外のディスクリプタを取得するために
		dev_->CreateRenderTargetView(
			backBuffers_[idx].Get(),
			&rtvDesc,
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

void DX12::CreateView(void)
{
	// 出力先の幅(ピクセル数)
	viewport_.Width = window_width;
	// 出力先の高さ(ピクセル数)
	viewport_.Height = window_height;
	// 出力先の左上座標X
	viewport_.TopLeftX = 0;
	// 出力先の左上座標Y
	viewport_.TopLeftY = 0;
	// 深度最大値
	viewport_.MaxDepth = 1.0f;
	// 深度最小値
	viewport_.MinDepth = 0.0f;
}

void DX12::CreateScissor(void)
{
	// 切り抜き上座標
	scissorrect_.top = 0;
	// 切り抜き左座標
	scissorrect_.left = 0;
	// 切り抜き右座標
	scissorrect_.right = scissorrect_.left + window_width;
	// 切り抜き下座標
	scissorrect_.bottom = scissorrect_.top + window_height;
}

void DX12::LoadModel(void)
{
	FILE* fp;
	auto error = fopen_s(&fp, "Resource/Model/Model/初音ミク.pmd", "rb");
	if (fp == nullptr)
	{
		char strerror[256];
		strerror_s(strerror, 256, error);
		MessageBox(hwnd_, strerror, "Open Error", MB_ICONERROR);
		return;
	}
	fread(signature_, sizeof(signature_), 1, fp);
	fread(&pmdHeader, sizeof(pmdHeader), 1, fp);

	fread(&vertNum_, sizeof(vertNum_), 1, fp);
	// バッファーの確保
	vertices_.resize(vertNum_);
	// 頂点の読み込み
	for (auto i = 0; i < vertNum_; i++)
	{
		fread(&vertices_[i], pmdvertexSize_, 1, fp);
	}
	//// 頂点の読み込み
	//fread(vertices_.data(), vertices_.size(), 1, fp);
	// インデックス数
	fread(&indicesNum, sizeof(indicesNum), 1, fp);
	auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(vertices_.size() * sizeof(PMD_VERTEX));

	auto result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff_.ReleaseAndGetAddressOf()));

	// 頂点情報のコピー
	PMD_VERTEX* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	copy(begin(vertices_), end(vertices_), vertMap);
	vertBuff_->Unmap(0, nullptr);

	// バッファーの仮想アドレス
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// 全バイト数
	vbView_.SizeInBytes = static_cast<UINT>(vertices_.size() * sizeof(PMD_VERTEX));
	// １頂点あたりのバイト数
	vbView_.StrideInBytes = sizeof(PMD_VERTEX);

	// モデルのインデックス情報を
	// インデックス
	indices_.resize(indicesNum);
	fread(indices_.data(), indices_.size() * sizeof(indices_[0]), 1, fp);
	fclose(fp);
	heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	resDesc = CD3DX12_RESOURCE_DESC::Buffer(indices_.size() * sizeof(indices_[0]));

	result = dev_->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(idxBuff_.ReleaseAndGetAddressOf()));
	// インデックスデータをコピー
	unsigned short* mappedIdx = nullptr;
	idxBuff_->Map(0, nullptr, (void**)&mappedIdx);
	copy(indices_.begin(), indices_.end(), mappedIdx);
	idxBuff_->Unmap(0, nullptr);
	// インデックスバッファビューを作成
	ibVIew_.BufferLocation = idxBuff_->GetGPUVirtualAddress();
	ibVIew_.Format = DXGI_FORMAT_R16_UINT;
	ibVIew_.SizeInBytes = static_cast<UINT>(indices_.size() * sizeof(indices_[0]));
}

void DX12::CreateVertices(void)
{
	LoadModel();

	// 頂点シェーダーの設定
	auto result = D3DCompileFromFile(
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
	// グラフィックスパイプラインステートの作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	// ルートシグネイチャーの設定
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
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// 論理演算は使用しない
	renderTargetBlendDesc.LogicOpEnable = false;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"TEXCOORD",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"BONE_NO",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"WEIGHT",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},

		{"EDGE_FLG",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};

	// 入力レイアウトの設定
	// レイアウト先頭アドレス
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	//レイアウト配列の要素数
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	// カットなし
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	// 三角形で構成
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// レンダーターゲットの設定
	// 今は１つのみ
	gpipeline.NumRenderTargets = 1;
	// ０～１に正規化されたRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// サンプル数設定
	// サンプリングは１ピクセルにつき１
	gpipeline.SampleDesc.Count = 1;
	// クオリティーは最低
	gpipeline.SampleDesc.Quality = 0;

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// テクスチャと定数バッファを１つずつの場合
	D3D12_DESCRIPTOR_RANGE descTblRange[2] = {};
	// テクスチャ用レジスター０番
	// テクスチャ１つ
	descTblRange[0].NumDescriptors = 1;
	// 種別はテクスチャ
	descTblRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 0番スロットから
	descTblRange[0].BaseShaderRegister = 0;
	descTblRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	// 定数用レジスター０番
	// 定数１つ
	descTblRange[1].NumDescriptors = 1;
	// 種別は定数
	descTblRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	// 0番スロットから
	descTblRange[1].BaseShaderRegister = 0;
	descTblRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootparam = {};
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// 配列先頭アドレス
	rootparam.DescriptorTable.pDescriptorRanges = descTblRange;
	// ディスクリプタレンジ数
	rootparam.DescriptorTable.NumDescriptorRanges = 2;
	// すべてのシェーダーから見える
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// ルートパラメーター数
	rootSignatureDesc.NumParameters = 1;
	// ルートパラメーターの先頭アドレス
	rootSignatureDesc.pParameters = &rootparam;


	// サンプラーの設定
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	// 横方向の繰り返し
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 縦方向の繰り返し
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// 奥方向の繰り返し
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// ボーダーは黒
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	// 線形保管
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
	// ミップマップ最大値
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	// ミップマップ最小値
	samplerDesc.MinLOD = 0.0f;
	// ピクセルシェーダーから見える
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// リサンプリングしない
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	result = D3D12SerializeRootSignature(&rootSignatureDesc,	// ルートシグネイチャ設定
		D3D_ROOT_SIGNATURE_VERSION_1_0,							// ルートシグネイチャのバージョン
		rootSigBlob_.ReleaseAndGetAddressOf(),
		errorBlob_.ReleaseAndGetAddressOf());

	result = dev_->CreateRootSignature(0,
		rootSigBlob_->GetBufferPointer(),
		rootSigBlob_->GetBufferSize(),
		IID_PPV_ARGS(rootSig_.ReleaseAndGetAddressOf()));
	gpipeline.pRootSignature = rootSig_.Get();

	// グラフィックスパイプラインステートオブジェクトの生成
	result = dev_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate_.ReleaseAndGetAddressOf()));
	CreateView();
	CreateScissor();
	// WICテクスチャのロード
	TexMetadata metadata = {};
	ScratchImage scratchImg = {};

	result = LoadFromWICFile(
		L"Resource/Image/textest.png", WIC_FLAGS_NONE,
		&metadata, scratchImg);

	auto img = scratchImg.GetImage(0, 0, 0);

	// WriteToSubresourceで転送するためのヒープ設定
	D3D12_HEAP_PROPERTIES HeapProp = {};
	// 特殊な設定なのでDEFAULTでもUPLOADでもない
	HeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ライトバック
	HeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// 転送はL0、つまりCPU側から直接行う
	HeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// 単一アダプターのための0
	HeapProp.CreationNodeMask = 0;
	HeapProp.VisibleNodeMask = 0;
	// リソース設定
	D3D12_RESOURCE_DESC resDesc = {};
	// RGBAフォーマット
	resDesc.Format = metadata.format;
	//resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 幅
	resDesc.Width = metadata.width;
	//resDesc.Width = 256;
	// 高さ
	resDesc.Height = metadata.height;
	//resDesc.Height = 256;
	// 2Dで配列でもないので1
	resDesc.DepthOrArraySize = metadata.arraySize;
	//resDesc.DepthOrArraySize = 1;
	// 通常テクスチャなのでアンチエイリアシングしない
	resDesc.SampleDesc.Count = 1;
	// クオリティーは最低
	resDesc.SampleDesc.Quality = 0;
	// ミップマップしないのでミップ数は１つ
	resDesc.MipLevels = metadata.mipLevels;;
	//resDesc.MipLevels = 1;
	// 2Dテクスチャ用
	resDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
	//resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// レイアウトは決定しない
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// 特にフラグなし
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	result = dev_->CreateCommittedResource(
		&HeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(texbuff_.ReleaseAndGetAddressOf()));
	result = texbuff_->WriteToSubresource(
		0,										// サブリソースインデックス
		nullptr,								// 全領域へのコピー
		img->pixels,							// 元データアドレス
		img->rowPitch,							// １ラインサイズ
		img->slicePitch							// 全サイズ
	);
	// 定数バッファの作成
	worldMat = XMMatrixIdentity();
	//worldMat = XMMatrixRotationY(XM_PIDIV4);
	// 目の位置
	XMFLOAT3 eye(0.0f, 10.0f, -15.0f);
	// 目の注視点
	XMFLOAT3 target(0.0f, 10.0f, 0.0f);
	// 目の上方向
	XMFLOAT3 up(0.0f, 1.0f, 0.0f);
	viewMat = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
	projMat = XMMatrixPerspectiveFovLH(
		XM_PIDIV2,																// 画角は90度
		static_cast<float>(window_width) / static_cast<float>(window_height),	// アスペクト比
		1.0f,																	// 近いほう
		100.0f);																	// 遠いほう

	auto buffheap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto buffdesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(cbuff) + 0xff) & ~0xff);
	dev_->CreateCommittedResource(
		&buffheap,
		D3D12_HEAP_FLAG_NONE,
		&buffdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(constBuff_.ReleaseAndGetAddressOf()));

	// マップ
	result = constBuff_->Map(0, nullptr, (void**)&mapBuff_);
	// 行列の内容をコピー
	mapBuff_->worldMat = worldMat;
	mapBuff_->viewMat = viewMat * projMat;
	for (int i = 0; i < 100; i++)
	{
		Wave& w = mapBuff_->waves[i];
		float randomRad = (float)(0.1f * 3.1415 * 2 * 0.3f);
		w.dir.x = sinf(randomRad);
		w.dir.y = cosf(randomRad);
		w.amplitude = 0.03f + powf(2.0f, 0.8f * 2.0f) * 0.05;
		w.waveLenght = 1.0f + powf(2.0f, 0.07f + 0.03) * 10.0f;
	}

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// シェーダーから見えるように
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// マスクは0
	descHeapDesc.NodeMask = 0;
	// SRV1つとCBV1つ
	descHeapDesc.NumDescriptors = 2;
	// シェーダーリソースビュー用
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// 生成
	result = dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap_));
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	// RGBA
	srvDesc.Format = metadata.format;
	//srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// 2Dテクスチャ
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// ミップマップは使用しないので１
	srvDesc.Texture2D.MipLevels = 1;
	// ディスクリプタの先頭ハンドルを取得しておく
	auto basicHeapHandle = basicDescHeap_->GetCPUDescriptorHandleForHeapStart();
	dev_->CreateShaderResourceView(
		texbuff_.Get(),										// ビューと関連付けるバッファー
		&srvDesc,											// 設定したテクスチャ設定情報
		basicDescHeap_->GetCPUDescriptorHandleForHeapStart()	// ヒープのどこに割り当てるか
	);
	// 次の場所に移動
	basicHeapHandle.ptr += dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
	cbvDesc.BufferLocation = constBuff_->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = constBuff_->GetDesc().Width;

	// 定数バッファービューの作成
	dev_->CreateConstantBufferView(&cbvDesc, basicHeapHandle);
}
