#include "Precompiled.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool() : stop(false)
{
    for (size_t i = 0; i < MAX_THREADS; ++i)
    {
        mWorkerPool.emplace_back([this]
            {
                while (true)
                {
                    std::function<void()> mTask;

                    {
                        std::unique_lock<std::mutex> mLock(mQueueMutex);
                        mQueueVariable.wait(mLock, [this]
                            {
                                return stop || !mTasks.empty();
                            });

                        if (stop && mTasks.empty())
                        {
                            return;
                        }

                        mTask = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    mTask(); // Execute the task
                }
            });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        stop = true;
    }

	mQueueVariable.notify_all();
	for (auto& thread : mWorkerPool)
	{
		thread.join(); // Join all threads to main thread from the worker thread pool
	}
}