

#include "Core/SystemInterface.h"
#include "Core/EventsManager.h"
#include <thread>
#include <string>
#include <unordered_map>

#ifndef MULTITHREADING_H
#define MULTITHREADING_H


#define THREADS ThreadsManager::Instance()

SINGLETON(ThreadsManager)
{
public:
	/*******************************************************************************
	/*!
	*
		\brief
			Initialise the system at engine start
	*/
	/*******************************************************************************/
	void Init();
	/*******************************************************************************
	/*!
	*
		\brief
			Runs when engine exits
	*/
	/*******************************************************************************/
	void Exit();

	/*******************************************************************************
	/*!
	*
		\brief
			Runs when engine exits
		\param _name
			Thread name
		\param _thread
			Thread to keep track of
	*/
	/*******************************************************************************/    
	template <typename... Args>
	std::thread& AddThread(Args&&... args)
	{
		return threads.emplace_back(std::forward<Args>(args)...);
	}

	/*******************************************************************************
	/*!
	*
		\brief
			Gets a mutex for a thread to run its critical section
		\param mutexType
			Type of mutex to acquire
		\return
			True if acquired
	*/
	/*******************************************************************************/
	bool AcquireMutex(const std::string & mutexName);

	/*******************************************************************************
	/*!
	*
		\brief
			Returns a mutex
		\param mutexType
			Type of mutex to return
	*/
	/*******************************************************************************/
	void ReturnMutex(const std::string & mutexName);

	/*******************************************************************************
	/*!
	*
		\brief
			Checks whether thread system has been told to exit
		\return
			Returns the value of quit member variable
	*/
	/*******************************************************************************/
	bool HasQuit() const;
private:
	std::vector<std::thread> threads;
	std::unordered_map<std::string, bool> mutexes;
	bool quit = false;
};

#endif // !MULTITHREADING_H