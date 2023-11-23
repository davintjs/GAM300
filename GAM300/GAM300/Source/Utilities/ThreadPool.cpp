/*!***************************************************************************************
\file			ThreadPool.cpp
\project
\author			Zacharie Hong, Davin Tan

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
    This file contains a threadpool that has threads waiting for work

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "Precompiled.h"
#include "ThreadPool.h"
#include <Windows.h>

void ThreadPool::Init()
{
    for (size_t i = 0; i < MAX_THREADS; ++i)
    {
        mWorkerPool.emplace_back
        (
            [this,i]
            {
                //Threads are constantly asking for jobs
                while (true)
                {
                    std::function<void()> mTask;
                    {
                        auto func = [this] { return stop || !mTasks.empty(); };
                        ACQUIRE_UNIQUE_LOCK(Queue,func);
                        if (stop && mTasks.empty())
                            return; // Thread should exit
                        mTask = std::move(mTasks.front());
                        mTasks.pop();
                    }
                    //PRINT("Thread ", i, " acquired job!\n");
                    mTask(); // Execute the task
                    //PRINT("Thread ", i, " finished job!\n");
                }
                //PRINT("Thread ",i, " exited\n");
            }
        );
    }
}

void ThreadPool::Exit()
{
    {
        ACQUIRE_SCOPED_LOCK(Queue);
        stop = true;
        //Clear all tasks
        mTasks={};
    }

	for (auto& thread : mWorkerPool)
	{
        CancelSynchronousIo(thread.native_handle());
		thread.join(); // Join all threads to main thread from the worker thread pool
	}
}

bool ThreadPool::HasStopped() const
{
    return stop;
}

ThreadPool::ScopedLock::ScopedLock(Mutex& _mutex) : mutex{ _mutex },
std::scoped_lock<std::mutex>::scoped_lock(_mutex.m)
{
}

ThreadPool::ScopedLock::~ScopedLock()
{
    mutex.condition.notify_all();
}


ThreadPool::UniqueLock::UniqueLock(ThreadPool::Mutex& _mutex) : mutex{ _mutex },
std::unique_lock<std::mutex>::unique_lock(_mutex.m)
{
}



ThreadPool::UniqueLock ThreadPool::AcquireUniqueLock(M_LOCK mutexName)
{
    return mutexes[(size_t)mutexName];
}



void ThreadPool::Wait(ThreadPool::UniqueLock& lock, std::function<bool()> pFunc)
{
    lock.mutex.condition.wait(lock, pFunc);
}



ThreadPool::ScopedLock ThreadPool::AcquireScopedLock(M_LOCK mutexName)
{
    return mutexes[(size_t)mutexName];
}