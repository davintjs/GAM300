#include "Precompiled.h"
#include "ThreadPool.h"

ThreadPool::ThreadPool()
{
    for (size_t i = 0; i < MAX_THREADS; ++i)
    {
        std::thread* thread = new std::thread;
        mWorkerPool.push_back(std::move(*thread));
    }

    RunTask();
}

void ThreadPool::RunTask()
{
    for (auto& mThread : mWorkerPool)
    {
        while (true)
        {
            std::function<void()> mTask;

            std::unique_lock<std::mutex> mLock(mQueueMutex);
            mQueueVariable.wait(mLock, [this] 
                { 
                    return mTasks.empty();
                });

            if (mTasks.empty())
            {
                return;
            }

            mTask = std::move(mTasks.front());
            mTasks.pop();
            mTask(); // Execute the task
        }
    }
}

ThreadPool::~ThreadPool()
{
	std::unique_lock<std::mutex> lock(mQueueMutex);

	mQueueVariable.notify_all();
	for (auto& thread : mWorkerPool)
	{
		thread.join(); // Join all threads to main thread from the worker thread pool
	}
}