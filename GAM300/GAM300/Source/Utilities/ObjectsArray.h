#ifndef OBJECTS_ARRAY_H
#define OBJECTS_ARRAY_H
#include "SparseSet.h"
#include <bitset>

using ObjectIndex = size_t;

template <typename T, ObjectIndex N>
class ObjectsArray : public SparseSet<T, N>
{
public:
	bool GetActive(ObjectIndex index)
	{
		return activeObjectsBitset.test(index);
	}

	void SetActive(ObjectIndex index, bool val)
	{
		activeObjectsBitset.set(index, val);
	}

	std::bitset<N> activeObjectsBitset;
};	


#endif // !OBJECTS_ARRAY_H
