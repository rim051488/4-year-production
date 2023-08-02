#pragma once
#include <memory>
#include "Common/Time.h"
#include "Common/Random.h"
#include "Common/Math.h"

#define lpSceneMng (SceneManager::GetInstance())
#define Rand (SceneManager::GetInstance().GetRandomClass())
#define Thread (SceneManager::GetInstance().GetThredPool())

class ResourceMng;
class BaseScene;
class ThreadPool;
class Controller;

class SceneManager
{
public:

	/// <summary>
	/// 実行処理
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	int Run(void);

	void Draw();

	void Update();

	static SceneManager& GetInstance(void)
	{
		return instance_;
	}

	/// <summary>
	/// リソースの管理クラスの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ResourceMng& GetResourceMng(void);

	/// <summary>
	/// タイムクラスの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	TimeClass& GetTimeClass(void);

	/// <summary>
	/// 乱数クラスの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Random& GetRandomClass(void);

	/// <summary>
	/// スレッドプール
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	ThreadPool& GetThredPool(void);

	/// <summary>
	/// プログラムを終了する
	/// </summary>
	/// <param name=""></param>
	void End(void)
	{
		isEnd_ = true;
	}

	// スクリーンサイズ
	template<ValueC C>
	static constexpr Vector2Tmp<C> screenSize_{ 1280,720 };

	/// <summary>
	/// コントローラーの取得
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	Controller& GetController(void);
private:
	SceneManager();
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name=""></param>
	/// <returns> 成功時、true失敗時、false </returns>
	bool Init(void);

	/// <summary>
	/// システム系の初期化
	/// </summary>
	/// <param name=""></param>
	/// <returns> 成功時、true失敗時、false </returns>
	bool SysInit(void);

	// このクラスのインスタンス
	static SceneManager instance_;

	// 初期化できたかのフラグ
	bool isInit_;

	// 終了するかのフラグ
	bool isEnd_;

	// リソースマネージャー
	std::unique_ptr<ResourceMng> resourceMng_;

	// タイムクラス
	TimeClass timeClass_;

	// ランダムクラス
	Random random_;

	// シーン
	std::unique_ptr<BaseScene> scene_;

	// スレッドプール
	std::unique_ptr<ThreadPool> threadPool_;

	// コントローラー
	std::unique_ptr<Controller> controller_;
	
};

