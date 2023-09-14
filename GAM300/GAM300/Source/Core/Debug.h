#include <cassert>
#include <iostream>
#include <typeinfo>

#ifndef DEBUG_H
#define DEBUG_H
#define ASSERT(expr) assert(expr);

template <typename... Args>
void PRINT(Args&&... args)
{
	((std::cout << std::forward<Args>(args)),...);
}

#endif