#pragma once
#include <mutex>
#include <list>
#include <functional>
#include <condition_variable>
#include <memory>
#include <vector>
class Worker;

// あらかじめスレッドを作成して非同期実行できるようにするクラス
class ThreadPool
{
public:
	ThreadPool(unsigned int threadNum = 1);
	~ThreadPool();

	/// <summary>
	/// 実行する関数を追加する
	/// </summary>
	/// <param name="func"> 実行したい関数 </param>
	void Add(std::function<void(void)>&& func);

	/// <summary>
	/// スレッドを終了待ちする
	/// </summary>
	/// <param name=""></param>
	void Join(void);

	/// <summary>
	/// Listの中身を破棄して実行中の処理を終了るまで待つ
	/// </summary>
	/// <param name=""></param>
	void Wait(void);

	/// <summary>
	/// すべてのタスクが終わるまで待機する
	/// </summary>
	/// <param name=""></param>
	void WaitAllTask(void);

	bool GetTask(Worker& woerker);


private:

	// ミューテックス
	std::mutex mutex_;

	// 実行するタスク
	std::list<std::function<void(void)>> taskList_;

	// 作業スレッドクラスを複数持つ
	std::vector<std::unique_ptr< Worker>> workers_;
};

