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

void ThreadsManager::init()
{
	quit = false;
	for (int i = 0; i < int(MutexType::None); ++i)
	{
		mutexes.emplace(std::make_pair(MutexType(i), 0));
	}
}

void ThreadsManager::update()
{

}

void ThreadsManager::addThread(std::thread* _thread)
{
	threads.push_back(_thread);
}

void ThreadsManager::exit()
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