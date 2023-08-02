#pragma once
#include <thread>
#include <functional>
#include <atomic>
#include <condition_variable>
#include <mutex>

class ThreadPool;

// 作業用スレッドを持つクラス
class Worker
{
public:
	Worker(ThreadPool& threadPool);
	~Worker();

	/// <summary>
	/// タスク(実行するfunction)をセットする
	/// </summary>
	/// <param name="task"> タスク </param>
	void SetTask(std::function<void(void)>&& task);

	/// <summary>
	/// 処理を停止する
	/// </summary>
	/// <param name=""></param>
	void Stop(void);

	/// <summary>
	/// Joinする
	/// </summary>
	/// <param name=""></param>
	void Join(void);

	/// <summary>
	/// タスクが終了しているか
	/// </summary>
	/// <param name=""></param>
	/// <returns> 終了true、失敗時false </returns>
	bool isTaskEnd(void);

	/// <summary>
	/// タスクが終了するまで待つ
	/// </summary>
	/// <param name=""></param>
	void Wait(void);
private:

	/// <summary>
	/// 実行用内部関数(スレッドで実行)
	/// </summary>
	/// <param name=""></param>
	void Run(void);

	// タスク(処理するやつ)
	std::function<void(void)> task_;

	// 実行するかのフラグ
	std::atomic_bool isRun_;

	// タスクが終了したかのフラグ
	std::atomic_bool isTaskEnd_;

	// ミューテックス
	std::mutex mutex_;

	// 条件変数
	std::condition_variable cd_;

	// スレッドプールクラスの参照
	ThreadPool& threadPool_;

	// 作業用スレッド
	std::thread workerThread_;
};

