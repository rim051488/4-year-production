#include <fstream>
#include "DX12.h"

DX12::DX12(HWND hwnd, int window_widht, int window_height) :
	window_hwnd_(hwnd),window_width_(),window_height_(window_height)
{
	UINT flagsDXGI = 0;
	ID3D12Debug* debug = nullptr;
	HRESULT hr{};
#if _DEBUG
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (debug)
	{
		debug->EnableDebugLayer();
		debug->Release();
	}
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG
	// ファクトリーの生成
	CreateFactory(hr,flagsDXGI);
	// デバイスの生成
	CreateDevice(hr);
	// コマンド類の生成
	CreateCommand(hr);
	// レンダーターゲットの生成
	CreateRenderTargetView(hr);
	// リソースの初期化
	resourceSetup();
	return;
}	

	

DX12::~DX12()
{
	if (vertex_shader.binaryptr)
	{
		free(vertex_shader.binaryptr);
	}
	if (pixel_shader.binaryptr)
	{
		free(pixel_shader.binaryptr);
	}
	CloseHandle(fence_event_);
}

HRESULT DX12::Render()
{
	// ビューポートの設定
	float clearColor[4] = { 1.0f,0.0f,1.0f,0.0f };
	D3D12_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)window_width_;
	viewport.Height = (float)window_height_;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;

	int targetIndex = swap_chain_->GetCurrentBackBufferIndex();
	
	SetResourceBarrier(
		command_list_.Get(),
		render_target_[targetIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);

	// Renderターゲットのクリア処理
	command_list_->RSSetViewports(1, &viewport);
	command_list_->ClearRenderTargetView(rtv_handle_[targetIndex], clearColor, 0, nullptr);

	D3D12_RECT rect = { 0,0,window_width_,window_height_ };
	command_list_->RSSetScissorRects(1, &rect);
	command_list_->OMSetRenderTargets(1, &rtv_handle_[targetIndex], true, nullptr);

	// 頂点データをセット
	command_list_->SetGraphicsRootSignature(root_signature_.Get());
	// シェーダー設定
	command_list_->SetPipelineState(pipeline_state_.Get());
	command_list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	command_list_->IASetVertexBuffers(0, 1, &buffer_pos_);
	command_list_->DrawInstanced(3, 1, 0, 0);

	// Presentする前の準備
	SetResourceBarrier(
		command_list_.Get(),
		render_target_[targetIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	command_list_->Close();

	// 積んだコマンドの実行
	ID3D12CommandList* pCommandList = command_list_.Get();
	command_queue_->ExecuteCommandLists(1, &pCommandList);
	swap_chain_->Present(1, 0);

	WaitForCommandQueue(command_queue_.Get());
	command_allocator_->Reset();
	command_list_->Reset(command_allocator_.Get(), nullptr);
	return S_OK;
}

void DX12::CreateFactory(HRESULT hr, UINT flagsDXGI)
{
	hr = CreateDXGIFactory2(flagsDXGI, IID_PPV_ARGS(factory_.ReleaseAndGetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	ComPtr<IDXGIAdapter> adapter;
	hr = factory_->EnumAdapters(0, adapter.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
}

void DX12::CreateDevice(HRESULT hr)
{
	// デバイス生成
	ComPtr<IDXGIAdapter> adapter;
	hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
}

void DX12::CreateCommand(HRESULT hr)
{
	// コマンドアロケータを生成
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(command_allocator_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	// コマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC desc_command_queue;
	ZeroMemory(&desc_command_queue, sizeof(desc_command_queue));
	desc_command_queue.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc_command_queue.Priority = 0;
	desc_command_queue.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	hr = device_->CreateCommandQueue(&desc_command_queue, IID_PPV_ARGS(command_queue_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	// コマンドキュー用のフェンスを準備
	fence_event_ = CreateEvent(nullptr, false, false, nullptr);
	hr = device_->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(queue_fence_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	// コマンドリストの作成
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator_.Get(), nullptr, IID_PPV_ARGS(command_list_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
}

void DX12::CreateRenderTargetView(HRESULT hr)
{
	// スワップチェインを生成
	DXGI_SWAP_CHAIN_DESC desc_swap_chain;
	ZeroMemory(&desc_swap_chain, sizeof(desc_swap_chain));
	desc_swap_chain.BufferCount = 2;
	desc_swap_chain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc_swap_chain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc_swap_chain.OutputWindow = window_hwnd_;
	desc_swap_chain.SampleDesc.Count = 1;
	desc_swap_chain.Windowed = true;
	desc_swap_chain.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	desc_swap_chain.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	hr = factory_->CreateSwapChain(command_queue_.Get(), &desc_swap_chain, (IDXGISwapChain**)swap_chain_.GetAddressOf());
	if (FAILED(hr))
	{
		return;
	}
	// ディスクリプタヒープ(RenderTarget用)の作成
	D3D12_DESCRIPTOR_HEAP_DESC desc_heap;
	ZeroMemory(&desc_heap, sizeof(desc_heap));
	desc_heap.NumDescriptors = 2;
	desc_heap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc_heap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device_->CreateDescriptorHeap(&desc_heap, IID_PPV_ARGS(descriptor_heap_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	// レンダーターゲット(プライマリ)の作成
	UINT strideHandleBytes = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (UINT i = 0; i < desc_swap_chain.BufferCount; ++i)
	{
		hr = swap_chain_->GetBuffer(i, IID_PPV_ARGS(render_target_[i].GetAddressOf()));
		if (FAILED(hr))
		{
			return;
		}
		rtv_handle_[i] = descriptor_heap_->GetCPUDescriptorHandleForHeapStart();
		rtv_handle_[i].ptr += static_cast<unsigned long long>(i) * strideHandleBytes;
		device_->CreateRenderTargetView(render_target_[i].Get(), nullptr, rtv_handle_[i]);
	}
	// Resourceの初期化
	
}

void DX12::SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	D3D12_RESOURCE_BARRIER descBarrier;
	ZeroMemory(&descBarrier, sizeof(descBarrier));
	descBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	descBarrier.Transition.pResource = resource;
	descBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	descBarrier.Transition.StateBefore = before;
	descBarrier.Transition.StateAfter = after;
	commandList->ResourceBarrier(1, &descBarrier);
}

void DX12::WaitForCommandQueue(ID3D12CommandQueue* pCommandQueue)
{
	static UINT64 frames = 0;
	queue_fence_->SetEventOnCompletion(frames, fence_event_);
	pCommandQueue->Signal(queue_fence_.Get(), frames);
	WaitForSingleObject(fence_event_, INFINITE);
	frames++;
}

bool DX12::resourceSetup()
{
	HRESULT hr{};
	// 頂点情報
	MyVertex vertices_array[] =
	{
		{0.0f,1.0f,0.0f},
		{-1.0f,-1.0f,0.0f},
		{1.0f,-1.0f,0.0f},
	};
	// PipelineStateのためのRootSignatureの作成
	D3D12_ROOT_SIGNATURE_DESC desc_root_signature;
	ZeroMemory(&desc_root_signature, sizeof(desc_root_signature));

	desc_root_signature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	ComPtr<ID3DBlob> root_sig_blob, error_blob;
	hr = D3D12SerializeRootSignature(&desc_root_signature, D3D_ROOT_SIGNATURE_VERSION_1, root_sig_blob.GetAddressOf(), error_blob.GetAddressOf());
	hr = device_->CreateRootSignature(0, root_sig_blob->GetBufferPointer(), root_sig_blob->GetBufferSize(), IID_PPV_ARGS(root_signature_.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// コンパイル済みのシェーダーを読み込み
	FILE* fpVS = nullptr;
	fopen_s(&fpVS, "Resource/Shader/VertexShader.vso", "rb");
	if (!fpVS)
	{
		return false;
	}
	fseek(fpVS, 0, SEEK_END);
	vertex_shader.size = ftell(fpVS);
	rewind(fpVS);
	vertex_shader.binaryptr = malloc(vertex_shader.size);
	fread(vertex_shader.binaryptr, 1, vertex_shader.size, fpVS);
	fclose(fpVS);
	fpVS = nullptr;
	FILE* fpPS = nullptr;
	fopen_s(&fpPS, "Resource/Shader/PixelShader.pso", "rb");
	if (!fpPS)
	{
		return false;
	}
	fseek(fpPS, 0, SEEK_END);
	pixel_shader.size = ftell(fpPS);
	rewind(fpPS);
	pixel_shader.binaryptr = malloc(pixel_shader.size);
	fread(pixel_shader.binaryptr, 1, pixel_shader.size,fpPS);
	fclose(fpPS);
	fpPS = nullptr;
	// 頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC desc_input_elements[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
	};
	// pipelineStatepのオジェクトの作成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_pipeline_state;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_state;
	ZeroMemory(&desc_pipeline_state, sizeof(desc_pipeline_state));
	desc_pipeline_state.VS.pShaderBytecode = vertex_shader.binaryptr;
	desc_pipeline_state.VS.BytecodeLength = vertex_shader.size;
	desc_pipeline_state.PS.pShaderBytecode = pixel_shader.binaryptr;
	desc_pipeline_state.PS.BytecodeLength = pixel_shader.size;
	desc_pipeline_state.SampleDesc.Count = 1;
	desc_pipeline_state.SampleMask = UINT_MAX;
	desc_pipeline_state.InputLayout.pInputElementDescs = desc_input_elements;
	desc_pipeline_state.InputLayout.NumElements = _countof(desc_input_elements);
	desc_pipeline_state.pRootSignature = root_signature_.Get();
	desc_pipeline_state.NumRenderTargets = 1;
	desc_pipeline_state.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc_pipeline_state.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	desc_pipeline_state.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	desc_pipeline_state.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	desc_pipeline_state.RasterizerState.DepthClipEnable = true;
	desc_pipeline_state.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	for (int i = 0; i < _countof(desc_state.BlendState.RenderTarget); ++i)
	{
		desc_pipeline_state.BlendState.RenderTarget[i].BlendEnable = false;
		desc_pipeline_state.BlendState.RenderTarget[i].SrcBlend = D3D12_BLEND_ONE;
		desc_pipeline_state.BlendState.RenderTarget[i].DestBlend = D3D12_BLEND_ZERO;
		desc_pipeline_state.BlendState.RenderTarget[i].BlendOp = D3D12_BLEND_OP_ADD;
		desc_pipeline_state.BlendState.RenderTarget[i].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc_pipeline_state.BlendState.RenderTarget[i].DestBlendAlpha = D3D12_BLEND_ZERO;
		desc_pipeline_state.BlendState.RenderTarget[i].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc_pipeline_state.BlendState.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	}
	desc_pipeline_state.DepthStencilState.DepthEnable = false;

	hr = device_->CreateGraphicsPipelineState(&desc_pipeline_state, IID_PPV_ARGS(pipeline_state_.GetAddressOf()));
	if (FAILED(hr))
	{
		return false;
	}

	// 頂点データの作成
	D3D12_HEAP_PROPERTIES heap_props;
	D3D12_RESOURCE_DESC desc_resource;
	ZeroMemory(&heap_props, sizeof(heap_props));
	ZeroMemory(&desc_resource, sizeof(desc_resource));
	heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heap_props.CreationNodeMask = 0;
	heap_props.VisibleNodeMask = 0;
	desc_resource.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc_resource.Width = sizeof(vertices_array);
	desc_resource.Height = 1;
	desc_resource.DepthOrArraySize = 1;
	desc_resource.MipLevels = 1;
	desc_resource.Format = DXGI_FORMAT_UNKNOWN;
	desc_resource.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc_resource.SampleDesc.Count = 1;
	
	hr = device_->CreateCommittedResource(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&desc_resource,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertex_buffer_.GetAddressOf())
	);
	if (FAILED(hr))
	{
		return false;
	}
	if (!vertex_buffer_)
	{
		return false;
	}
	// 頂点データの書き込み
	void* mapped = nullptr;
	hr = vertex_buffer_->Map(0, nullptr, &mapped);
	if (SUCCEEDED(hr))
	{
		memcpy(mapped, vertices_array, sizeof(vertices_array));
		vertex_buffer_->Unmap(0, nullptr);
	}
	if (FAILED(hr))
	{
		return false;
	}
	buffer_pos_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
	buffer_pos_.StrideInBytes = sizeof(MyVertex);
	buffer_pos_.SizeInBytes = sizeof(vertices_array);
	return true;
}
