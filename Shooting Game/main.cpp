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
		// ウィンドウクラスの初期化
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

	// ウィンドウのサイズを決定
	RECT rect = {
		(LONG)0,
		(LONG)0,
		(LONG)(WINDOW_WIDTH),
		(LONG)(WINDOW_HEIGHT)
	};

	// ウィンドウサイズの補正
	AdjustWindowRect(
		&rect,											// クライアント矩形
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION,		// ウィンドウスタイル
		false											// メニューフラグ
	);
	// ウィンドウ作成
	hwnd = CreateWindow(_T(CLASS_NAME),					// クラス名指定
		_T(PROC_NAME),									// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,							// タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,									// 表示 X 座標は OS にお任せします
		CW_USEDEFAULT,									// 表示 Y 座標は OS にお任せします
		rect.right - rect.left,							// ウィンドウの幅
		rect.bottom - rect.top,							// ウィンドウの高さ
		nullptr,										// 親ウィンドウハンドル
		nullptr,										// メニューハンドル
		hInstance,										// 呼び出しアプリケーションハンドル
		nullptr											// 追加ハンドル
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
