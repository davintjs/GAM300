#include "Precompiled.h"
#include "ThreadPool.h"
#include <Windows.h>

void ThreadPool::Init()
{
    for (size_t i = 0; i < MAX_THREADS; ++i)
    {
        mWorkerPool.emplace_back
        (
            [this]
            {
                while (true)
                {
                    std::function<void()> mTask;
                    {
                        ACQUIRE_UNIQUE_LOCK
                        (
                            "Queue",
                            [this]
                            {
                                return stop || !mTasks.empty();
                            }
                        );

                        if (stop && mTasks.empty())
                        {
                            return;
                        }
                        mTask = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    mTask(); // Execute the task
                }
            }
        );
    }
}

void ThreadPool::Exit()
{
    stop = true;
    for (auto pair : mutexes)
    {
        ACQUIRE_SCOPED_LOCK(pair.first);
    }

	for (auto& thread : mWorkerPool)
	{
        CancelSynchronousIo(thread.native_handle());
		thread.join(); // Join all threads to main thread from the worker thread pool
	}

    //Delete Mutexes
    for (auto& pair : mutexes)
    {
        delete pair.second;
    }
}