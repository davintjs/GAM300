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
                while (!stop)
                {
                    std::function<void()> mTask;
                    {
                        ACQUIRE_SCOPED_LOCK(Queue);
                        if (mTasks.empty())
                            continue;
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

    //Delete Mutexes
    for (auto& pair : mutexes)
    {
        delete pair.second;
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



ThreadPool::UniqueLock ThreadPool::AcquireUniqueLock(std::string mutexName)
{
    if (mutexes.find(mutexName) == mutexes.end())
    {
        // Mutex doesn't exist, so create it and add it to the map
        mutexes.emplace(mutexName, new Mutex());
    }
    Mutex* mutex = mutexes[mutexName];
    return UniqueLock(*mutex);
}



void ThreadPool::Wait(ThreadPool::UniqueLock& lock, std::function<bool()> pFunc)
{
    lock.mutex.condition.wait(lock, pFunc);
}



ThreadPool::ScopedLock ThreadPool::AcquireScopedLock(std::string mutexName)
{
    if (mutexes.find(mutexName) == mutexes.end())
    {
        // Mutex doesn't exist, so create it and add it to the map
        mutexes.emplace(mutexName, new Mutex());
    }
    Mutex* mutex = mutexes[mutexName];
    return ScopedLock(*mutex);
}