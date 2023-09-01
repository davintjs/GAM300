#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>

class ThreadPool
{
public:
	ThreadPool() = delete;
	ThreadPool(int numThreads);
	~ThreadPool();

	template <typename T>
	void EnqueueTask(T&& task)
	{
		std::unique_lock<std::mutex> mLock(mQueueMutex);
		mTasks.emplace(std::move(task));

		mQueueVariable.notify_one();
	}

	void RunTask();

private:
	std::vector<std::thread> mWorkerPool; // Worker threads pool
	std::queue<std::function<void()>> mTasks; // Queue of tasks for worker threads to complete

	std::mutex mQueueMutex;
	std::condition_variable mQueueVariable;
};