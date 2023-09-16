#include "Handle.h"
#include "Debugging/Debugger.h"
#include <functional>

template <typename T>
Handle<T>::Handle(Engine::UUID _uuid, T& _object) :uuid{ _uuid }, pObject{ &_object }{}

template <typename T>
T& Handle<T>::Get()
{
	E_ASSERT(pObject,"Object is nullptr");
	return *pObject;
}