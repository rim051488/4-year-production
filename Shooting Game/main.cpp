#include <Windows.h>
#include <vector>
#ifdef _DEBUG
#include <iostream>
#endif
#include "DX12/DX12.h"


const unsigned int window_width = 1280;
const unsigned int window_height = 720;

using namespace std;

// ウィンドウプロシージャの作成
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// ウィンドウが破棄されたときによばれる
	if (msg == WM_DESTROY)
	{
		PostQuitMessage(0);	// OSに対して終わることを伝える
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);	// 既定の処理を行う
}

// デバッグレイヤー
void EnableDebugLayer()
{
	ID3D12Debug* debugLayer = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugLayer))))
	{
		debugLayer->EnableDebugLayer();
		debugLayer->Release();
	}
}

void DebugOutputFormatString(const char* format,...)
{
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf_s(valist);
	va_end(valist);
#endif
}

#ifdef _DEBUG
int main()
{
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
#endif
#ifdef _DEBUG
	// デバッグレイヤーをオンに
	EnableDebugLayer();
#endif // _DEBUG
	DebugOutputFormatString("Show window test.");
	// ウィンドウクラスの生成&登録
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	// コールバック関数の指定
	w.lpfnWndProc = (WNDPROC)WindowProcedure;
	// アプリケーションクラス名
	w.lpszClassName = "ShootingGame";
	// ハンドルの取得
	w.hInstance = GetModuleHandle(nullptr);
	// アプリケーションクラス(ウィンドウクラスの指定をOSに伝える)
	RegisterClassEx(&w);

	// ウィンドウサイズを決める
	RECT wrc = { 0,0,window_width ,window_height };

	// 関数を使ってウィンドウのサイズを補正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウィンドウオブジェクトの生成
	HWND hwnd = CreateWindow(w.lpszClassName,	// クラス名指定
		"ShootingGame",							// タイトルバーの文字
		WS_OVERLAPPEDWINDOW,					// タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,							// 表示x座標はOSに任せる
		CW_USEDEFAULT,							// 表示y座標はOSに任せる
		wrc.right - wrc.left,					// ウィンドウの幅
		wrc.bottom - wrc.top,					// ウィンドウの高さ
		nullptr,								// 親ウィンドウハンドル
		nullptr,								// メニューハンドル
		w.hInstance,							// 呼び出しアプリケーションハンドル
		nullptr);								// 追加パラメータ

	unique_ptr<DX12> dx12;
	dx12 = make_unique<DX12>(hwnd);

	// ウィンドウの表示
	ShowWindow(hwnd, SW_SHOW);
	MSG msg = {};
	while (true)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// アプリケーションが終わるときにmessageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}
		dx12->Render();
	}
	// もうクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);
	return 0;
}

