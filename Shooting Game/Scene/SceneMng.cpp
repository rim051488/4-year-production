#include "SceneMng.h"

// �E�B���h�E�v���V�[�W���̍쐬
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// �E�B���h�E���j�����ꂽ�Ƃ��ɂ�΂��
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	// OS�ɑ΂��ďI��邱�Ƃ�`����
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	// ����̏������s��
}

void SceneMng::Run(void)
{
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// �A�v���P�[�V�������I���Ƃ���message��WM_QUIT�ɂȂ�
		if (msg.message == WM_QUIT)
		{
			break;
		}
		dx12->Render();
	}
	// �����N���X�͎g��Ȃ��̂œo�^��������
	UnregisterClass(w.lpszClassName, w.hInstance);
}

bool SceneMng::SysInit(void)
{
#ifdef _DEBUG
	// �f�o�b�O���C���[���I����
	EnableDebugLayer();
#endif // _DEBUG
	DebugOutputFormatString("Show window test.");
	// �E�B���h�E�N���X�̐���&�o�^
	w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	// �R�[���o�b�N�֐��̎w��
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	// �A�v���P�[�V�����N���X��
	w.lpszClassName = "ShootingGame";
	// �n���h���̎擾
	w.hInstance = GetModuleHandle(nullptr);
	// �A�v���P�[�V�����N���X(�E�B���h�E�N���X�̎w���OS�ɓ`����)
	RegisterClassEx(&w);

	// �E�B���h�E�T�C�Y�����߂�
	RECT wrc = { 0,0,window_width ,window_height };

	// �֐����g���ăE�B���h�E�̃T�C�Y��␳����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// �E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,	// �N���X���w��
		"ShootingGame",							// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,					// �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,							// �\��x���W��OS�ɔC����
		CW_USEDEFAULT,							// �\��y���W��OS�ɔC����
		wrc.right - wrc.left,					// �E�B���h�E�̕�
		wrc.bottom - wrc.top,					// �E�B���h�E�̍���
		nullptr,								// �e�E�B���h�E�n���h��
		nullptr,								// ���j���[�n���h��
		w.hInstance,							// �Ăяo���A�v���P�[�V�����n���h��
		nullptr);								// �ǉ��p�����[�^

	// directX12�̏�����
	dx12 = make_unique<DX12>(hwnd);
	// �E�B���h�E�̕\��
	ShowWindow(hwnd, SW_SHOW);
	// �V�F�[�_�[�Ȃǂ̏�����
	dx12->CreateVertices();
	return true;
}

void SceneMng::Draw(float delta)
{
}

SceneMng::SceneMng()
{
	InitFlag_ = SysInit();
}

SceneMng::~SceneMng()
{
}

// �f�o�b�O���C���[
void SceneMng::EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
	{
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}

void SceneMng::DebugOutputFormatString(const char* format, ...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	vprintf(format, valist);
	va_end(valist);
#endif
}
