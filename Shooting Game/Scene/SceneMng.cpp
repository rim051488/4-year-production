#include "SceneMng.h"

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
		// アプリケーションが終わるときにmessageがWM_QUITになる
		if (msg.message == WM_QUIT)
		{
			break;
		}
		dx12->Render();
	}
	// もうクラスは使わないので登録解除する
	UnregisterClass(w.lpszClassName, w.hInstance);
}

bool SceneMng::SysInit(void)
{
#ifdef _DEBUG
	// デバッグレイヤーをオンに
	EnableDebugLayer();
#endif // _DEBUG
	DebugOutputFormatString("Show window test.");
	// ウィンドウクラスの生成&登録
	w = {};
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

	// directX12の初期化
	dx12 = make_unique<DX12>(hwnd);
	// ウィンドウの表示
	ShowWindow(hwnd, SW_SHOW);
	// シェーダーなどの初期化
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

// デバッグレイヤー
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
