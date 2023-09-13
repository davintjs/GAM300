#include <iostream>

#ifndef  DEBUG_H
#define DEBUG_H


#if defined(DEBUG) | defined(_DEBUG)
	template <typename... Args>
	static void PRINT(Args&&... args)
	{
		((std::cout << std::forward<Args>(args)), ...);
	}
#else
	#define PRINT(ARGS) 

#endif
#endif // ! DEBUG_H