#include "ThreadPool.h"
#include "Worker.h"
#include <deque>

#include "Debug.h"

ThreadPool::ThreadPool(unsigned int threadNum)
{
	workers_.resize(threadNum);
	for (auto& w : workers_)
	{
		w = std::make_unique<Worker>(*this);
	}
}

ThreadPool::~ThreadPool()
{
	Join();
}

// 非同期で実行したい関数を追加する
void ThreadPool::Add(std::function<void(void)>&& func)
{
	// ロックする
	std::lock_guard<std::mutex> lock(mutex_);

	// リストに追加
	taskList_.push_back(std::move(func));

	for (auto& w : workers_)
	{
		if (w->isTaskEnd())
		{
			// タスク終了しているworkerがいたらタスクをセットする
			w->SetTask(std::move(*taskList_.begin()));
			taskList_.pop_front();
			return;
		}
	}

}

void ThreadPool::Join(void)
{
	for (auto& w : workers_)
	{
		// workerをjoinする
		w->Join();
	}
}


// 処理中ののみ待つ
void ThreadPool::Wait(void)
{
	for (auto& w : workers_)
	{
		// workerの処理を待つ
		w->Wait();
	}
}


void ThreadPool::WaitAllTask(void)
{
	while (true)
	{
		if (taskList_.empty())
		{
			// タスクリストが殻になったら抜ける
			break;
		}

		// 他スレッドに処理の機会を与える
		std::this_thread::yield();
	}

	// 現在の処理中のタスクを終了まで待つ
	Wait();
}

bool ThreadPool::GetTask(Worker& worker)
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (taskList_.empty())
	{
		// タスクがないとき
		return false;
	}

	// タスクをセットする
	worker.SetTask(std::move(*taskList_.begin()));
	taskList_.pop_front();
	DebugLog("ゲット");
	return true;
}

