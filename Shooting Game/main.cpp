#include <Windows.h>
#include <tchar.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#define CLASS_NAME "CLASS TEST01"
#define PROC_NAME "ShootingGame"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPTSTR lpCmdLine, int nCmdShow)
{
	HWND			hwnd;
	MSG				msg;
	if (!hPreInst)
	{
		// �E�B���h�E�N���X�̏�����
		WNDCLASS	my_prog;
		my_prog.style = CS_HREDRAW | CS_VREDRAW;
		my_prog.lpfnWndProc = WndProc;
		my_prog.cbClsExtra = 0;
		my_prog.cbWndExtra = 0;
		my_prog.hInstance = hInstance;
		my_prog.hIcon = nullptr;
		my_prog.hCursor = LoadCursor(nullptr, IDC_ARROW);
		my_prog.hbrBackground = nullptr;
		my_prog.lpszMenuName = nullptr;
		my_prog.lpszClassName = _T(CLASS_NAME);

		if (!RegisterClass(&my_prog))
		{
			return false;
		}
	}

	// �E�B���h�E�̃T�C�Y������
	RECT rect = {
		(LONG)0,
		(LONG)0,
		(LONG)(WINDOW_WIDTH),
		(LONG)(WINDOW_HEIGHT)
	};

	// �E�B���h�E�T�C�Y�̕␳
	AdjustWindowRect(
		&rect,											// �N���C�A���g��`
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,		// �E�B���h�E�X�^�C��
		false											// ���j���[�t���O
	);
	// �E�B���h�E�쐬
	hwnd = CreateWindow(_T(CLASS_NAME),					// �N���X���w��
		_T(PROC_NAME),									// �^�C�g���o�[�̕���
		WS_OVERLAPPEDWINDOW,							// �^�C�g���o�[�Ƌ��E��������E�B���h�E
		CW_USEDEFAULT,									// �\�� X ���W�� OS �ɂ��C�����܂�
		CW_USEDEFAULT,									// �\�� Y ���W�� OS �ɂ��C�����܂�
		rect.right - rect.left,							// �E�B���h�E�̕�
		rect.bottom - rect.top,							// �E�B���h�E�̍���
		nullptr,										// �e�E�B���h�E�n���h��
		nullptr,										// ���j���[�n���h��
		hInstance,										// �Ăяo���A�v���P�[�V�����n���h��
		nullptr											// �ǉ��n���h��
	);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	do 
	{
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	} while (msg.message != WM_QUIT);
	return (int)(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hwnd, msg, wParam, lParam));
	}
	return (0L);
}
