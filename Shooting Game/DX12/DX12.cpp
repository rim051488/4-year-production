#include <vector>
#include <string>
#include "DX12.h"

DX12::DX12(HWND hwnd)
{
	hwnd_ = hwnd;
	frame = 0;
	// �f�o�C�X�Ȃǂ̏�����
	CreateDvice();
	CreateCommand();
	CreateCommandQueue();
	CreateSwapChain();
	CreateDescriptor();
}

DX12::~DX12(void)
{
	// �e�N�X�`���f�B�X�N���v�^�̏�����
	texDescHeap->Release();
}

void DX12::Render(void)
{
	// �o�b�N�o�b�t�@�̃C���f�b�N�X���擾
	auto bbIdx = swapchain_->GetCurrentBackBufferIndex();

	// �o���A�̏���
	D3D12_RESOURCE_BARRIER BarrierDesc = {};
	// �J��
	BarrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// �w��Ȃ�
	BarrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// �o�b�N�o�b�t�@���\�[�X
	BarrierDesc.Transition.pResource = backBuffers_[bbIdx].Get();
	BarrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	// ���O��PRESENT���
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// �����烌���_�[�^�[�Q�b�g���
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// �o���A�̎w����s
	cmdList_->ResourceBarrier(1, &BarrierDesc);

	// �����_�[�^�[�Q�b�g���w��
	auto rtvH = rtvHeaps_->GetCPUDescriptorHandleForHeapStart();
	rtvH.ptr += static_cast<ULONG_PTR>(bbIdx * dev_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
	cmdList_->OMSetRenderTargets(1, &rtvH, false, nullptr);

	// ��ʃN���A
	float r, g, b;
	r = 255/*(float)(0xff & frame >> 16) / 255.0f*/;
	g = 0/*(float)(0xff & frame >> 8) / 255.0f*/;
	b = 0/*(float)(0xff & frame >> 0) / 255.0f*/;
	float clearColor[] = { r,g,b,1.0f };
	cmdList_->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	++frame;
	cmdList_->RSSetViewports(1, &viewport_);
	cmdList_->RSSetScissorRects(1, &scissorrect_);
	cmdList_->SetPipelineState(pipelinestate_.Get());
	cmdList_->SetGraphicsRootSignature(rootSig_.Get());
	cmdList_->SetDescriptorHeaps(1,  &texDescHeap);
	//cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	 cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);
	cmdList_->IASetIndexBuffer(&ibVIew_);
	// �C���f�b�N�X���g�p����ꍇ
	//cmdList_->DrawIndexedInstanced(6, 1, 0, 0, 0);
	cmdList_->DrawInstanced(4, 1, 0, 0);

	cmdList_->SetGraphicsRootDescriptorTable(
		0,// ���[�g�p�����[�^�[�C���f�b�N�X
		texDescHeap->GetGPUDescriptorHandleForHeapStart());// �q�[�v�A�h���X

	// �O�ゾ�������ւ���
	BarrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	BarrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	cmdList_->ResourceBarrier(1, &BarrierDesc);

	// ���߂̃N���[�Y
	cmdList_->Close();

	// �R�}���h���X�g�̎��s
	ID3D12CommandList* cmdlists = cmdList_.Get();
	cmdQueue_->ExecuteCommandLists(1, &cmdlists);

	// �҂�
	cmdQueue_->Signal(fence_.Get(), ++fenceVal_);
	if (fence_->GetCompletedValue() != fenceVal_)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// �L���[���N���A
	cmdAllcator_->Reset();
	// �R�}���h���X�g�����߂鏀��
	cmdList_->Reset(cmdAllcator_.Get(), nullptr);

	// �t���b�v
	swapchain_->Present(1, 0);
}

void DX12::CreateDvice(void)
{
	dev_ = nullptr;
	dxgiFactory_ = nullptr;
	// �f�o�C�X�̏�����������O�ɃO���{�������𒲂ׂ�
	if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
	{
		if (FAILED(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(dxgiFactory_.ReleaseAndGetAddressOf()))))
		{
			return;
		}
	};

	// �A�_�v�^�[�̗񋓗p
	vector<IDXGIAdapter*> adapters;

	// �����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0; dxgiFactory_->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; i++)
	{
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		// �A�_�v�^�[�̐����I�u�W�F�N�g�擾
		adpt->GetDesc(&adesc);

		std::wstring strDesc = adesc.Description;

		// �T�������A�_�v�^�[�̖��O���m�F
		if (strDesc.contains(L"NVIDIA") || strDesc.contains(L"AMD"))
		{
			tmpAdapter = adpt;
			break;
		}
	}

	// �f�o�C�X�̏�����
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
		// �����\���ǂ����𒲂ׂ�
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
	// �R�}���h�A���P�[�^�[�̏�����
	auto result = dev_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmdAllcator_.ReleaseAndGetAddressOf()));
	// �R�}���h���X�g�̏�����
	result = dev_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAllcator_.Get(), nullptr, IID_PPV_ARGS(cmdList_.ReleaseAndGetAddressOf()));
}

void DX12::CreateSwapChain(void)
{
	swapchain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	// �摜�𑜓x��
	swapchainDesc.Width = window_width;
	// �摜�𑜓x��
	swapchainDesc.Height = window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �X�e���I�\���t���O
	swapchainDesc.Stereo = false;
	// �}���`�T���v���̎w��(Count�͊�{�͂P)
	swapchainDesc.SampleDesc.Count = 1;
	// �}���`�T���v���̎w��(Quality�͊�{�͂O)
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	// �_�u���o�b�t�@�Ȃ̂łQ
	swapchainDesc.BufferCount = 2;

	// �o�b�N�o�b�t�@�͐L�яk�݉\
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	// �t���b�v��͑��₩�ɔj��
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// �w��Ȃ�
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// �E�B���h�E�̃t���X�N���[���̐؂�ւ��\
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

	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;			// �����_�[�^�[�Q�b�g�r���[�Ȃ̂�RTV
	heapDesc.NodeMask = 0;
	heapDesc.NumDescriptors = 2;							// �\���̂Q��
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;		// �w��Ȃ�

	rtvHeaps_ = nullptr;

	auto result = dev_->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeaps_.ReleaseAndGetAddressOf()));

	// �X���b�v�`�F�[���ƃ������̂Ђ��t��
	result = swapchain_->GetDesc(&swcDesc);
	backBuffers_.resize(swcDesc.BufferCount);
	D3D12_CPU_DESCRIPTOR_HANDLE handle = rtvHeaps_->GetCPUDescriptorHandleForHeapStart();

	for (size_t idx = 0; idx < swcDesc.BufferCount; ++idx)
	{
		result = swapchain_->GetBuffer(static_cast<UINT>(idx), IID_PPV_ARGS(backBuffers_[idx].ReleaseAndGetAddressOf()));
		// �O�ԖڈȊO�̃f�B�X�N���v�^���擾���邽�߂�
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

	// �^�C���A�E�g����
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	// �A�_�v�^�[���P�����g��Ȃ����͂O�ł悢
	cmdQueueDesc.NodeMask = 0;
	// �v���C�I���e�B�͓��Ɏw��Ȃ�
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	// �R�}���h���X�g�ƍ��킹��
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	// �L���[�̐���
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
			OutputDebugStringA("�t�@�C������������܂���");
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
	// �o�͐�̕�(�s�N�Z����)
	viewport_.Width = window_width;
	// �o�͐�̍���(�s�N�Z����)
	viewport_.Height = window_height;
	// �o�͐�̍�����WX
	viewport_.TopLeftX = 0;
	// �o�͐�̍�����WY
	viewport_.TopLeftY = 0;
	// �[�x�ő�l
	viewport_.MaxDepth = 1.0f;
	// �[�x�ŏ��l
	viewport_.MinDepth = 0.0f;
}

void DX12::CreateScissor(void)
{
	// �؂蔲������W
	scissorrect_.top = 0;
	// �؂蔲�������W
	scissorrect_.left = 0;
	// �؂蔲���E���W
	scissorrect_.right = scissorrect_.left + window_width;
	// �؂蔲�������W
	scissorrect_.bottom = scissorrect_.top + window_height;
}

void DX12::CreateVertices(void)
{
	// ���_�f�[�^�\����
	struct Vertex
	{
		// xyz���W
		XMFLOAT3 pos;
		// uv���W
		XMFLOAT2 uv;
	};

	// ���_�o�b�t�@�[�̐���
	Vertex vertices[] =
	{
		{{-0.4f,-0.7f,0.0f},{0.0f,1.0f}},	// ����
		{{-0.4f,0.7f,0.0f},{0.0f,0.0f}},	// ����
		{{0.4f,-0.7f,0.0f},{1.0f,1.0f}},	// �E��
		{{0.4f,0.7f,0.0f},{1.0f,0.0f}},		// �E��
	};

	// ���_�q�[�v�̐ݒ�
	D3D12_HEAP_PROPERTIES heapprop = {};
	// �q�[�v�̎��
	heapprop.Type = D3D12_HEAP_TYPE_UPLOAD;
	// CPU�̃y�[�W���O�ݒ�
	heapprop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	// �������v�[�����ǂ���������
	heapprop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resdesc = {};
	// �o�b�t�@�Ŏg���̂�BUFFER���w��
	resdesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	// ���őS���܂��Ȃ��̂�sizeof(�S���_)�Ƃ���
	resdesc.Width = sizeof(vertices);
	// ���ŕ\�����Ă���̂�1�Ƃ���
	resdesc.Height = 1;
	// �P�ł悢
	resdesc.DepthOrArraySize = 1;
	// �P�ł悢
	resdesc.MipLevels = 1;
	// �摜�ł͂Ȃ��̂�UNKNOWN�ł悢
	resdesc.Format = DXGI_FORMAT_UNKNOWN;
	// SampleDesc.Count = 1�Ƃ���
	resdesc.SampleDesc.Count = 1;
	// D3D12_TEXTURE_LAYOUT_ROW_MAJOR�Ƃ���
	resdesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	// NONE�ł悢
	resdesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	auto result = dev_->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&resdesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vertBuff_.ReleaseAndGetAddressOf()));

	// ���_���̃R�s�[
	Vertex* vertMap = nullptr;

	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);

	copy(begin(vertices), end(vertices), vertMap);

	vertBuff_->Unmap(0, nullptr);
	// �o�b�t�@�[�̉��z�A�h���X
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	// �S�o�C�g��
	vbView_.SizeInBytes = sizeof(vertices);
	// �P���_������̃o�C�g��
	vbView_.StrideInBytes = sizeof(vertices[0]);

	//// �C���f�b�N�X�̐���
	//unsigned short indices[] =
	//{
	//	0,1,2,
	//	2,1,3
	//};
	//ID3D12Resource* idxBuff = nullptr;
	//// �ݒ�́A�o�b�t�@�̃T�C�Y�ȊO�A���_�o�b�t�@�̐ݒ���g���܂킵�ł���
	//result = dev_->CreateCommittedResource(
	//	&heapprop,
	//	D3D12_HEAP_FLAG_NONE,
	//	&resdesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	IID_PPV_ARGS(&idxBuff));
	//// ������o�b�t�@�ɃC���f�b�N�X�f�[�^���R�s�[
	//unsigned short* mappedIdx = nullptr;
	//idxBuff->Map(0, nullptr, (void**)&mappedIdx);
	//copy(begin(indices), end(indices), mappedIdx);
	//idxBuff->Unmap(0, nullptr);
	//// �C���f�b�N�X�o�b�t�@�[�r���[���쐬
	//ibVIew_.BufferLocation = idxBuff->GetGPUVirtualAddress();
	//ibVIew_.Format = DXGI_FORMAT_R16_UINT;
	//ibVIew_.SizeInBytes = sizeof(indices);

	// ���_�V�F�[�_�[�̐ݒ�
	result = D3DCompileFromFile(
		L"Shader/VS.hlsl",														// �V�F�[�_�[��
		nullptr,																// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,										// �C���N���[�h�̓f�t�H���g
		"BasicVS", "vs_5_0",													// �֐���BaasicVS�A�ΏۃV�F�[�_�[��vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,						// �f�o�b�O�p�y�эœK���Ȃ�
		0,
		vsBlob_.ReleaseAndGetAddressOf(), errorBlob_.ReleaseAndGetAddressOf());	// �R���p�C����������vsBlob_�ɁA���s����errorBlob_�Ɋi�[
	// �s�N�Z���V�F�[�_�[�̐ݒ�
	result = D3DCompileFromFile(
		L"Shader/PS.hlsl",														// �V�F�[�_�[��
		nullptr,																// define�͂Ȃ�
		D3D_COMPILE_STANDARD_FILE_INCLUDE,										// �C���N���[�h�̓f�t�H���g
		"BasicPS", "ps_5_0",													// �֐���BaasicVS�A�ΏۃV�F�[�_�[��vs_5_0
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,						// �f�o�b�O�p�y�эœK���Ȃ�
		0,
		psBlob_.ReleaseAndGetAddressOf(), errorBlob_.ReleaseAndGetAddressOf());	// �R���p�C����������psBlob_�ɁA���s����errorBlob_�Ɋi�[
	//FailedShader(result);
	// ���_���C�A�E�g�̐ݒ�
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ // ���W���
			"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
		{ // uv(�ǉ�)
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,
			0,D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};
	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�̍쐬
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline = {};
	// ���[�g�V�O�l�C�`���[�̐ݒ�
	gpipeline.pRootSignature = nullptr;
	// �V�F�[�_�[�̃Z�b�g
	// ���_�V�F�[�_�[�̐ݒ�
	gpipeline.VS.pShaderBytecode = vsBlob_->GetBufferPointer();
	gpipeline.VS.BytecodeLength = vsBlob_->GetBufferSize();
	// �s�N�Z���V�F�[�_�[�̐ݒ�
	gpipeline.PS.pShaderBytecode = psBlob_->GetBufferPointer();
	gpipeline.PS.BytecodeLength = psBlob_->GetBufferSize();
	// �T���v���}�X�N�ƃ��X�^���C�U�[�X�e�[�g�̐ݒ�
	// �f�t�H���g�̃T���v���}�X�N��\���萔(0xffffffff)
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	// �A���`�G�C���A�V���O�͎g��Ȃ��̂�false�ɂ���
	gpipeline.RasterizerState.MultisampleEnable = false;
	// �J�����O���Ȃ�
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	// ���g��h��Ԃ�
	gpipeline.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	// �[�x�����̃N���b�s���O�͗L����
	gpipeline.RasterizerState.DepthClipEnable = true;

	// �u�����h�X�e�[�g�̐ݒ�
	gpipeline.BlendState.AlphaToCoverageEnable = false;
	gpipeline.BlendState.IndependentBlendEnable = false;

	D3D12_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc = {};
	renderTargetBlendDesc.BlendEnable = false;
	renderTargetBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	// �_�����Z�͎g�p���Ȃ�
	renderTargetBlendDesc.LogicOpEnable = false;
	gpipeline.BlendState.RenderTarget[0] = renderTargetBlendDesc;
	
	// ���̓��C�A�E�g�̐ݒ�
	// ���C�A�E�g�擪�A�h���X
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	//���C�A�E�g�z��̗v�f��
	gpipeline.InputLayout.NumElements = _countof(inputLayout);
	// �J�b�g�Ȃ�
	gpipeline.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
	// �O�p�`�ō\��
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	// �����_�[�^�[�Q�b�g�̐ݒ�
	// ���͂P�̂�
	gpipeline.NumRenderTargets = 1;
	// �O�`�P�ɐ��K�����ꂽRGBA
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// �T���v�����ݒ�
	// �T���v�����O�͂P�s�N�Z���ɂ��P
	gpipeline.SampleDesc.Count = 1;
	// �N�I���e�B�[�͍Œ�
	gpipeline.SampleDesc.Quality = 0;

	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descTblRange = {};
	// �e�N�X�`���P��
	descTblRange.NumDescriptors = 1;
	// ��ʂ̓e�N�X�`��
	descTblRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	// 0�ԃX���b�g����
	descTblRange.BaseShaderRegister = 0;
	descTblRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	
	D3D12_ROOT_PARAMETER rootparam = {};
	// �s�N�Z���V�F�[�_�[���猩����
	rootparam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	// �s�N�Z���V�F�[�_�[���猩����
	rootparam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// �f�B�X�N���v�^�����W�̃A�h���X
	rootparam.DescriptorTable.pDescriptorRanges = &descTblRange;
	// �f�B�X�N���v�^�����W��
	rootparam.DescriptorTable.NumDescriptorRanges = 1;
	// ���[�g�p�����[�^�[�̐擪�A�h���X
	rootSignatureDesc.pParameters = &rootparam;
	// ���[�g�p�����[�^�[��
	rootSignatureDesc.NumParameters = 1;

	// �T���v���[�̐ݒ�
	D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
	// �������̌J��Ԃ�
	samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �c�����̌J��Ԃ�
	samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �������̌J��Ԃ�
	samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	// �{�[�_�[�͍�
	samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	// ���`�ۊ�
	samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	// �~�b�v�}�b�v�ő�l
	samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
	// �~�b�v�}�b�v�ŏ��l
	samplerDesc.MinLOD = 0.0f;
	// �s�N�Z���V�F�[�_�[���猩����
	samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	// ���T���v�����O���Ȃ�
	samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;

	result = D3D12SerializeRootSignature(&rootSignatureDesc,	// ���[�g�V�O�l�C�`���ݒ�
		D3D_ROOT_SIGNATURE_VERSION_1_0,							// ���[�g�V�O�l�C�`���̃o�[�W����
		rootSigBlob_.ReleaseAndGetAddressOf(),
		errorBlob_.ReleaseAndGetAddressOf());

	result = dev_->CreateRootSignature(0,
		rootSigBlob_->GetBufferPointer(),
		rootSigBlob_->GetBufferSize(),
		IID_PPV_ARGS(rootSig_.ReleaseAndGetAddressOf()));
	gpipeline.pRootSignature = rootSig_.Get();

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̐���
	result = dev_->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(pipelinestate_.ReleaseAndGetAddressOf()));
	CreateView();
	CreateScissor();
	// �e�N�X�`���f�[�^�̍쐬
	vector<TexRGBA> texturedata(256 * 256);
	for (auto& rgba : texturedata)
	{
		rgba.R = rand() % 256;
		rgba.G = rand() % 256;
		rgba.B = rand() % 256;
		rgba.A = 256;
	}
	// WriteToSubresource�œ]�����邽�߂̃q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES HeapProp = {};
	// ����Ȑݒ�Ȃ̂�DEFAULT�ł�UPLOAD�ł��Ȃ�
	HeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	// ���C�g�o�b�N
	HeapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	// �]����L0�A�܂�CPU�����璼�ڍs��
	HeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// �P��A�_�v�^�[�̂��߂�0
	HeapProp.CreationNodeMask = 0;
	HeapProp.VisibleNodeMask = 0;
	// ���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc = {};
	// RGBA�t�H�[�}�b�g
	resDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// ��
	resDesc.Width = 256;
	// ����
	resDesc.Height = 256;
	// 2D�Ŕz��ł��Ȃ��̂�1
	resDesc.DepthOrArraySize = 1;
	// �ʏ�e�N�X�`���Ȃ̂ŃA���`�G�C���A�V���O���Ȃ�
	resDesc.SampleDesc.Count = 1;
	// �N�I���e�B�[�͍Œ�
	resDesc.SampleDesc.Quality = 0;
	// �~�b�v�}�b�v���Ȃ��̂Ń~�b�v���͂P��
	resDesc.MipLevels = 1;
	// 2D�e�N�X�`���p
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	// ���C�A�E�g�͌��肵�Ȃ�
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	// ���Ƀt���O�Ȃ�
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	result = dev_->CreateCommittedResource(
		&HeapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		nullptr,
		IID_PPV_ARGS(texbuff_.ReleaseAndGetAddressOf()));
	result = texbuff_->WriteToSubresource(
		0,										// �T�u���\�[�X�C���f�b�N�X
		nullptr,								// �S�̈�ւ̃R�s�[
		texturedata.data(),						// ���f�[�^�A�h���X
		sizeof(TexRGBA) * 256,					// �P���C���T�C�Y
		sizeof(TexRGBA) * texturedata.size()	// �S�T�C�Y
	);
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	// �V�F�[�_�[���猩����悤��
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	// �}�X�N��0
	descHeapDesc.NodeMask = 0;
	// �r���[�͍��̂Ƃ���P����
	descHeapDesc.NumDescriptors = 1;
	// �V�F�[�_�[���\�[�X�r���[�p
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	// ����
	result = dev_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&texDescHeap));
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	// RGBA
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	// 
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	// 2D�e�N�X�`��
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	// �~�b�v�}�b�v�͎g�p���Ȃ��̂łP
	srvDesc.Texture2D.MipLevels = 1;
	dev_->CreateShaderResourceView(
		texbuff_.Get(),										// �r���[�Ɗ֘A�t����o�b�t�@�[
		&srvDesc,											// �ݒ肵���e�N�X�`���ݒ���
		texDescHeap->GetCPUDescriptorHandleForHeapStart()	// �q�[�v�̂ǂ��Ɋ��蓖�Ă邩
	);

	dev_->CreateShaderResourceView(texbuff_.Get(), //�r���[�Ɗ֘A�t����o�b�t�@
		&srvDesc, //��قǐݒ肵���e�N�X�`���ݒ���
		texDescHeap->GetCPUDescriptorHandleForHeapStart()//�q�[�v�̂ǂ��Ɋ��蓖�Ă邩
	);

}
