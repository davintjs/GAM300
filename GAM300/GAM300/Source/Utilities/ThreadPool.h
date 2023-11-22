/*!***************************************************************************************
\file			ThreadPool.h
\project
\author			Zacharie Hong, Davin Tan

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
	This file contains a threadpool that has threads waiting for work

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#pragma once

#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Core/SystemInterface.h"

constexpr int MAX_THREADS = 10;

//Easier singleton access
#define THREADS ThreadPool::Instance()

//Conditional variable lock
#define ACQUIRE_UNIQUE_LOCK(MUTEX_NAME,FUNC) ThreadPool::UniqueLock lock##MUTEX_NAME = ThreadPool::Instance().AcquireUniqueLock(#MUTEX_NAME); ThreadPool::Instance().Wait(lock##MUTEX_NAME,FUNC)

//Scoped lock
#define ACQUIRE_SCOPED_LOCK(MUTEX_NAME) ThreadPool::ScopedLock lock##MUTEX_NAME = ThreadPool::Instance().AcquireScopedLock(#MUTEX_NAME)

SINGLETON(ThreadPool)
{
public:
	//Creates all threads to wait for jobs
	void Init();
	//Signals all threads to exit
	void Exit();
	//Adds a task to be executed
	template <typename T>
	void EnqueueTask(T && task);
	//Whether this system has stopped
	bool HasStopped() const;

	struct Mutex
	{
		std::condition_variable condition;
		std::mutex m;
	};

	class ScopedLock : public std::scoped_lock<std::mutex>
	{
	public:
		//Constructor
		ScopedLock(Mutex& _mutex);
		//Destructor that notifies all
		~ScopedLock();
		Mutex& mutex;
	};

	class UniqueLock : public std::unique_lock<std::mutex>
	{
	public:
		//Constructor
		UniqueLock(Mutex& _mutex);
		Mutex& mutex;
	};

	//Try to get a unique lock
	UniqueLock AcquireUniqueLock(std::string mutexName);

	//Wait for a conditional variable
	void Wait(UniqueLock& lock, std::function<bool()> pFunc);

	//Try to get a scoped lock
	ScopedLock AcquireScopedLock(std::string mutexName);

private:
	std::vector<std::thread> mWorkerPool; // Worker threads pool
	std::queue<std::function<void()>> mTasks; // Queue of tasks for worker threads to complete

	std::unordered_map<std::string, Mutex*> mutexes;

	bool stop {false};
};

template <typename T>
void ThreadPool::EnqueueTask(T&& task)
{
	ACQUIRE_SCOPED_LOCK(Queue);
	mTasks.emplace(std::move(task));
}
