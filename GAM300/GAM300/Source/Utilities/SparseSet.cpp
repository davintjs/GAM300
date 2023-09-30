/*!***************************************************************************************
\file			sparse-set.cpp
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
    This file contains the function definitions for a fixed size SparseSet

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "SparseSet.h"
#include "Debugging/Debugger.h"


#pragma warning( disable : 26495) // Data is supposed to be uninitialized;

#define SPARSESET SparseSet<T, N>
#define ITERATOR SPARSESET::Iterator

#pragma region Iterator

template <typename T, ObjectIndex N>
ITERATOR::Iterator(ObjectIndex _sparseIndex, SparseSet<T, N>& _arr) : sparseIndex(_sparseIndex), arr{ _arr } {}

template <typename T, ObjectIndex N>
T& ITERATOR::operator*() const
{
    return arr[sparseIndex];
}

template <typename T, ObjectIndex N>
typename ITERATOR ITERATOR::operator++()
{
    ++sparseIndex;
    return *this;
}

template <typename T, ObjectIndex N>
typename ITERATOR ITERATOR::operator++(int)
{
    Iterator tmp(*this);
    operator++();
    return tmp;
}

template <typename T, ObjectIndex N>
bool ITERATOR::operator==(const Iterator& other) const {
    return sparseIndex == other.sparseIndex;
}

template <typename T, ObjectIndex N>
bool ITERATOR::operator!=(const Iterator& other) const {
    return sparseIndex != other.sparseIndex;
}

#pragma endregion

#pragma region SparseSet

template <typename T, ObjectIndex N>
typename ITERATOR SPARSESET::begin() { return Iterator(0, *this); }
template <typename T, ObjectIndex N>
typename ITERATOR SPARSESET::end() { return Iterator(size_, *this); }

template <typename T, ObjectIndex N>
SPARSESET::SparseSet()
{
    for (ObjectIndex i = 0; i < N; ++i)
    {
        nodes[i] = {i,i};
    }
}

template <typename T, ObjectIndex N>
SPARSESET::~SparseSet()
{
    for (T& element : *this)
    {
        element.~T();
    }
}

template <typename T, ObjectIndex N>
template <typename... Args>
T& SPARSESET::emplace_back(Args&&... args)
{
    T& back = *new (data + nodes[size_].denseIndex) T(args...); // Construct the new element in the array
    ++size_;
    return back;
}

template <typename T, ObjectIndex N>
template <typename... Args>
T& SPARSESET::emplace(ObjectIndex index, Args&&... args)
{
    //Find and make to next element
    E_ASSERT(index < N, " OUT OF BOUNDS");
    for (ObjectIndex i = size_; i < N; ++i)
    {
        if (nodes[i].denseIndex == index)
        {
            nodes[i].denseIndex = nodes[size_].denseIndex;
            nodes[size_].denseIndex = index;
            ++size_;
            return *new (data + index) T(args...); // Construct the new element in the array
        }
    }
    //Already exists
    return reinterpret_cast<T*>(data)[index];
}

template <typename T, ObjectIndex N>
void SPARSESET::erase(ObjectIndex denseIndex)
{
    for (ObjectIndex i = 0; i < size_; ++i)
    {
        if (nodes[i].denseIndex == denseIndex)
        {
            reinterpret_cast<T*>(data)[denseIndex].~T();
            --size_;
            nodes[i].denseIndex = nodes[size_].denseIndex;
            nodes[size_].denseIndex = denseIndex;
            return;
        }
    }
    E_ASSERT(false,"FAILED TO ERASE");
}

template <typename T, ObjectIndex N>
void SPARSESET::erase(T& val)
{
    erase(GetDenseIndex(val));
}

template <typename T, ObjectIndex N>
bool SPARSESET::contains(T& pValue)
{
    if (&pValue < reinterpret_cast<T*>(data))
        return false;
    if (&pValue - reinterpret_cast<T*>(data) >= N)
        return false;
    return true;
}

template <typename T, ObjectIndex N>
T& SPARSESET::operator[] (size_t i)
{
    E_ASSERT(i < size_, "ARRAY OUT OF BOUNDS");
    return *reinterpret_cast<T*>(data + nodes[i].denseIndex);
}

template <typename T, ObjectIndex N>
T& SPARSESET::DenseSubscript(ObjectIndex index)
{
    return reinterpret_cast<T*>(data)[index];
}

template <typename T, ObjectIndex N>
ObjectIndex SPARSESET::GetDenseIndex(T& object)
{
    return (ObjectIndex)(&object - reinterpret_cast<T*>(data));
}

template <typename T, ObjectIndex N>
ObjectIndex SPARSESET::GetDenseIndex(ObjectIndex sparseIndex)
{
    return nodes[sparseIndex].denseIndex;
}

template <typename T, ObjectIndex N>
ObjectIndex SPARSESET::size() const{return size_;}

template <typename T, ObjectIndex N>
void SPARSESET::clear() { size_ = 0; }

template <typename T, ObjectIndex N>
bool SPARSESET::empty() const { return !size_; }

template <typename T, ObjectIndex N>
bool SPARSESET::full() const { return size_ == N; }

template <typename T, ObjectIndex N>
T* SPARSESET::TryGetDense(ObjectIndex denseIndex) 
{
    if (denseIndex >= N)
        return nullptr;
    for (ObjectIndex i = 0; i < size_; ++i)
    {
        if (nodes[i].denseIndex == denseIndex)
            return reinterpret_cast<T*>(data)+denseIndex;
    }
    return nullptr;
};

template <typename T, ObjectIndex N>
bool SPARSESET::TryErase(T& object)
{
    if (!contains(object))
        return false;
    erase(object);
    return true;
}


template <typename T, ObjectIndex N>
bool SPARSESET::TryErase(ObjectIndex denseIndex)
{
    if (denseIndex >= N)
        return false;
    for (ObjectIndex i = 0; i < size_; ++i)
    {
        if (nodes[i].denseIndex == denseIndex)
        {
            reinterpret_cast<T*>(data)[denseIndex].~T();
            --size_;
            nodes[i].denseIndex = nodes[size_].denseIndex;
            nodes[size_].denseIndex = denseIndex;
            return true;
        }
    }
    return false;
}

#pragma endregion