#include "Worker.h"
#include "ThreadPool.h"

#include "Debug.h"

Worker::Worker(ThreadPool& threadPool) :
	threadPool_{threadPool}
{
	isRun_.store(true);
	isTaskEnd_.store(true);
	workerThread_ = std::thread{ &Worker::Run,this };
}

Worker::~Worker()
{
	Join();
}

void Worker::SetTask(std::function<void(void)>&& task)
{
	std::lock_guard<std::mutex> lock(mutex_);
	task_ = std::move(task);
	isTaskEnd_.store(false);
	cd_.notify_all();
}

void Worker::Stop(void)
{
	isRun_.store(false);
}

void Worker::Join(void)
{
	if (workerThread_.joinable())
	{
		// ジョイン可能な時実行フラグをfalseに
		isRun_.store(false);

		if (isTaskEnd_.load())
		{
			// タスクが終了しているとき待機用のタスクを入れる
			SetTask([]() {});
		}

		// joinする
		workerThread_.join();
	}
}

bool Worker::isTaskEnd(void)
{
	return isTaskEnd_.load();
}

void Worker::Wait(void)
{
	while (!isTaskEnd_.load())
	{
		// タスクが終了するまでこのスレッドをyieldしながら待つ
		std::this_thread::yield();
	}

}

void Worker::Run(void)
{
	while (isRun_.load())
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);

			// リストの中身が入るまで待機(既に入っていたら進む)
			cd_.wait(lock, [this]() { return task_; });

			// 実行する
			task_();
			task_ = nullptr;
			isTaskEnd_.store(true);
		}

		// タスク取得を試みる
		isTaskEnd_.store(!threadPool_.GetTask(*this));
		
	}
}
