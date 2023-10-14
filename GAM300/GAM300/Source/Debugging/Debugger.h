/*!***************************************************************************************
\file			Debugger.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			25/09/2023

\brief
	This file contains a the declarations for a debugger using SpeedLogger

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#pragma once
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <spdlog/spdlog.h>

#include <memory>
#include <iostream>
#include "Core/SystemInterface.h"

#ifndef  DEBUG_H
#define DEBUG_H

//Variadic template printing
#if defined(DEBUG) | defined(_DEBUG)
template <typename... Args>
inline void PRINT(Args&&... args)
{
	((std::cout << std::forward<Args>(args)), ...);
}
#else
#pragma warning( disable : 4002)
#define PRINT(ARGS) 

#endif
#endif // ! DEBUG_H

//Easy access for debugger
#define DEBUGGER Debugger::Instance()

SINGLETON(Debugger)
{
public:
	/***************************************************************************/
	/*!
	\brief
		Initialises all the loggers with their desired levels and formats
	*/
	/**************************************************************************/
	Debugger();

	/***************************************************************************/
	/*!
	\brief
		Used to log asserts to both file and to the console
	*/
	/**************************************************************************/
	void error_log();

	/***************************************************************************/
	/*!
	\brief
		Returns a string sent to the console
	\return
		the message as a string as well
	*/
	/**************************************************************************/
	std::string to_string(std::string msg);

	/***************************************************************************/
	/*!
	\brief
		Creates multiple rotating log files
	\param fileSize
		the maximum file size
	\param fileAmount
		the maximum amount of files
	*/
	/**************************************************************************/
	void create_multiplefile(int fileSize, int fileAmount);

	/***************************************************************************/
	/*!
	\brief
		Returns the consoleLogger
	\return
		the consoleLogger
	*/
	/**************************************************************************/
	inline std::shared_ptr<spdlog::logger>& getConsoleLogger() { return consoleLogger; }

private:
	std::shared_ptr<spdlog::logger> consoleLogger;
};

//User Macros
#define CONSOLE_CRITICAL(...){DEBUGGER.getConsoleLogger()->critical(__VA_ARGS__);}
#define CONSOLE_ERROR(...){DEBUGGER.getConsoleLogger()->error(__VA_ARGS__);}
#define CONSOLE_WARN(...){DEBUGGER.getConsoleLogger()->warn(__VA_ARGS__);}
#define CONSOLE_INFO(...){DEBUGGER.getConsoleLogger()->info(__VA_ARGS__);}
#define CONSOLE_TRACE(...){DEBUGGER.getConsoleLogger()->trace(__VA_ARGS__);}

#define FILE_CRITICAL(...)		::spdlog::critical(__VA_ARGS__);
#define FILE_ERROR(...)			::spdlog::error(__VA_ARGS__)
#define FILE_WARN(...)			::spdlog::warn(__VA_ARGS__)
#define FILE_INFO(...)			::spdlog::info(__VA_ARGS__)

//enter the condition to trigger the assert, followed by the message you want.
#define E_ASSERT(condition,...)\
        if (!(condition)) {       \
			DEBUGGER.error_log();\
            std::ostringstream assertMessageStream; \
			assertMessageStream << "\nCaused By:\t" << #condition << '\n' << "Info:\t\t";\
            appendCustomAssertMessage(assertMessageStream, __VA_ARGS__); \
			assertMessageStream << "\nSource:\t\t" << __FILE__ << " (Line: " << __LINE__ << ")\n";\
			FILE_CRITICAL(assertMessageStream.str());\
			spdlog::drop("Error");\
			abort();\
        }

// Helper function to append custom messages
template <typename T>
void appendCustomAssertMessage(std::ostringstream& stream, const T& arg) {
	stream << arg;
}

// Helper function to append custom messages
template <typename T, typename... Args>
void appendCustomAssertMessage(std::ostringstream& stream, const T& arg, const Args&... args) {
	stream << arg << " ";
	appendCustomAssertMessage(stream, args...);
}
#define Console_ToString(...)	DEBUGGER.to_string(__VA_ARGS__)