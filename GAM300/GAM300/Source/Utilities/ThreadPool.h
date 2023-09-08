#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Core/SystemInterface.h"

constexpr int MAX_THREADS = 6;

#define THREADS ThreadPool::Instance()

#define ACQUIRE_UNIQUE_LOCK(MUTEX_NAME,FUNC) ThreadPool::UniqueLock lock = ThreadPool::Instance().AcquireUniqueLock(MUTEX_NAME); ThreadPool::Instance().Wait(lock,FUNC)

#define ACQUIRE_SCOPED_LOCK(MUTEX_NAME) ThreadPool::ScopedLock lock = ThreadPool::Instance().AcquireScopedLock(MUTEX_NAME);

SINGLETON(ThreadPool)
{
public:
	void Init();
	void Exit();

	template <typename T>
	void EnqueueTask(T&& task)
	{
		{
			AcquireScopedLock("Queue");
			mTasks.emplace(std::move(task));
		}
	}

	bool HasStopped() const
	{
		return stop;
	}

	struct Mutex
	{
		std::condition_variable condition;
		std::mutex m;
	};

	class ScopedLock : public std::scoped_lock<std::mutex>
	{
	public:
		ScopedLock(Mutex& _mutex) : mutex{ _mutex },
			std::scoped_lock<std::mutex>::scoped_lock(_mutex.m)
		{
		}

		~ScopedLock()
		{
			mutex.condition.notify_all();
		}

		Mutex& mutex;
	};

	class UniqueLock : public std::unique_lock<std::mutex>
	{
	public:
		UniqueLock(Mutex& _mutex) : mutex{ _mutex },
			std::unique_lock<std::mutex>::unique_lock(_mutex.m)
		{
		}

		Mutex& mutex;
	};

	UniqueLock AcquireUniqueLock(std::string mutexName)
	{
		if (mutexes.find(mutexName) == mutexes.end())
		{
			// Mutex doesn't exist, so create it and add it to the map
			mutexes.emplace(mutexName, new Mutex());
		}
		Mutex* mutex = mutexes[mutexName];
		return UniqueLock(*mutex);
	}

	void Wait(UniqueLock& lock,std::function<bool()> pFunc)
	{
		lock.mutex.condition.wait(lock, pFunc);
	}

	ScopedLock AcquireScopedLock(std::string mutexName)
	{
		if (mutexes.find(mutexName) == mutexes.end())
		{
			// Mutex doesn't exist, so create it and add it to the map
			mutexes.emplace(mutexName, new Mutex());
		}
		Mutex* mutex = mutexes[mutexName];
		return ScopedLock(*mutex);
	}

private:
	std::vector<std::thread> mWorkerPool; // Worker threads pool
	std::queue<std::function<void()>> mTasks; // Queue of tasks for worker threads to complete

	std::unordered_map<std::string, Mutex*> mutexes;

	bool stop {false};
};