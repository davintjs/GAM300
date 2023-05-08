#include <cassert>
#include <iostream>
#include <typeinfo>

#ifndef DEBUG_H
#define DEBUG_H
#define ASSERT(expr) assert(expr);

template <typename Arg, typename... Args>
void PRINT(Arg&& arg, Args&&... args)
{
	std::cout << std::forward<Arg>(arg);
}

#endif