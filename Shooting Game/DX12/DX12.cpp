#include <vector>
#include <string>
#include "DX12.h"

DX12::DX12(void)
{
	swapchain_ = nullptr;
	// デバイスなどの初期化
	CreateDvice();
	CreateCommand();
	CreateCommandQueue();
}

DX12::~DX12(void)
{
}

void DX12::CreateDvice(void)
{
	dev_ = nullptr;
	dxgiFactory_ = nullptr;
	// デバイスの初期化をする前にグラボが何かを調べる
	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_));
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
		if (strDesc.find(L"NVIDIA") == string::npos)
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
		if (D3D12CreateDevice(tmpAdapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&dev_)) == S_OK)
		{
			featurelevel = level;
			break;
		}
	}
}

void DX12::CreateCommand(void)
{
	cmdAllcator_ = nullptr;
	cmdList_ = nullptr;
	// コマンドアロケーターの初期化
	auto result = dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAllcator_));
	// コマンドリストの初期化
	result = dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllcator_, nullptr, IID_PPV_ARGS(&cmdList_));
}

void DX12::CreateCommandQueue(void)
{
	cmdQueue = nullptr;
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
	auto result = dev_->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
}