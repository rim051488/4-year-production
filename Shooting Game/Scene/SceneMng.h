#pragma once
#include <Windows.h>
#include <vector>
#include <iostream>
#include "../DX12/DX12.h"

#define lpSceneMng SceneMng::GetInstance()
#define Rand (lpSceneMng.GetRandom())

class SceneMng
{
public:
	// 静的シングルトン
	static SceneMng& GetInstance()
	{
		static SceneMng s_Instance;
		return s_Instance;
	}
	// 実行処理
	void Run(void);

private:
	// ウィンドウのサイズ
	const unsigned int window_width = 1280;
	const unsigned int window_height = 720;

	// 初期化処理
	bool SysInit(void);
	void Draw(float delta);
	bool InitFlag_;

	// directX12の初期化
	unique_ptr<DX12> dx12;
	// ウィンドウの生成用変数
	WNDCLASSEX w;

	// デバッグレイヤー
	void EnableDebugLayer();
	void DebugOutputFormatString(const char* format, ...);

	SceneMng();
	~SceneMng();

};

