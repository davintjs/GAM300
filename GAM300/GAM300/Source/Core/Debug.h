#include <iostream>

#ifndef  DEBUG_H
#define DEBUG_H


#if defined(DEBUG) | defined(_DEBUG)
	#define PRINT(ARGS) printMessage(ARGS)

	template <typename... Args>
	static void printMessage(Args&&... args)
	{
		((std::cout << std::forward<Args>(args)), ...);
	}
#else
	#define PRINT(ARGS) 

#endif
#endif // ! DEBUG_H