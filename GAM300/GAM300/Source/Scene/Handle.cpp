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

template <typename T>
std::size_t Handle<T>::operator()() const
{
	return std::hash(uuid);
}

template <typename T>
bool Handle<T>::operator==(const Handle<T>& rhs) const
{
	return uuid == rhs.uuid;
}