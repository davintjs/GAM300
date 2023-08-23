#pragma once

#include "Core/SystemInterface.h"
#include "Core/EventsManager.h"
#include <thread>
#include <unordered_map>


enum class MutexType
{
	FileSystem,
	None
};

ENGINE_EDITOR_SYSTEM(ThreadsManager)
{
public:
	/*******************************************************************************
	/*!
	*
		\brief
			Initialise the system at engine start
	*/
	/*******************************************************************************/
	void init();
	/*******************************************************************************
	/*!
	*
		\brief
			Runs at engine update
	*/
	/*******************************************************************************/
	void update();
	/*******************************************************************************
	/*!
	*
		\brief
			Runs when engine exits
	*/
	/*******************************************************************************/
	void exit();

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
	void addThread(std::thread * _thread);

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
	bool acquireMutex(MutexType mutexType)
	{
		if (mutexes[mutexType] == 1)
			return 0;
		mutexes[mutexType] = 1;
		return 1;
	}

	/*******************************************************************************
	/*!
	*
		\brief
			Returns a mutex
		\param mutexType
			Type of mutex to return
	*/
	/*******************************************************************************/
	void returnMutex(MutexType mutexType)
	{
		mutexes[mutexType] = 0;
	}

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
	std::vector<std::thread*> threads;
	std::unordered_map<MutexType, bool> mutexes;
	bool quit = false;
};