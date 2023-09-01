/*!***************************************************************************************
\file			thread-system.cpp
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			26/09/2022

\brief
	Defines Thread-system class functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "Precompiled.h"
#include "MultiThreading.h"

void ThreadsManager::Init()
{
	quit = false;
}

void ThreadsManager::AddThread(std::thread* _thread)
{
	threads.push_back(_thread);
}

bool ThreadsManager::AcquireMutex(const std::string& mutexName)
{
	if (mutexes[mutexName] == 1)
		return 0;
	mutexes[mutexName] = 1;
	return 1;
}

void ThreadsManager::ReturnMutex(const std::string& mutexName)
{
	mutexes[mutexName] = 0;
}

void ThreadsManager::Exit()
{
	quit = true;
	for (std::thread* thread : threads)
	{
		thread->join();
		delete thread;
	}
}

bool ThreadsManager::HasQuit() const
{
	return quit;
}