#include "DX12Resouce.h"

DX12Resouce::DX12Resouce(HWND hwnd, int window_widht, int window_height) :
	window_hwnd_(hwnd),window_width_(),window_height_(window_height)
{
	UINT flagsDXGI = 0;
	ID3D12Debug* debug = nullptr;
	HRESULT hr;
#if _DEBUG
	D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
	if (debug)
	{
		debug->EnableDebugLayer();
		debug->Release();
	}
	flagsDXGI |= DXGI_CREATE_FACTORY_DEBUG;
#endif // _DEBUG
	// GPU側の処理をするため？
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
	// デバイス生成
	hr = D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(device_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	// コマンドアロケータを生成
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(command_allocator_.GetAddressOf()));
	if (FAILED(hr))
	{
		return;
	}
	// コマンドキューを生成


}

DX12Resouce::~DX12Resouce()
{
}

HRESULT DX12Resouce::Render()
{
	// ビューポートの設定
	float clearColor[4] = { 1.0f,0.0f,0.0f,0.0f };
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

void DX12Resouce::SetResourceBarrier(ID3D12GraphicsCommandList* commandList, ID3D12Resource* resource, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
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

void DX12Resouce::WaitForCommandQueue(ID3D12CommandQueue* pCommandQueue)
{
	static UINT64 frames = 0;
	queue_fence_->SetEventOnCompletion(frames, fence_event_);
	pCommandQueue->Signal(queue_fence_.Get(), frames);
	WaitForSingleObject(fence_event_, INFINITE);
	frames++;
}
