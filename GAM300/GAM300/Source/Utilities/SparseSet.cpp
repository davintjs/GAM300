/*!***************************************************************************************
\file			sparse-set.cpp
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			27/06/2023

\brief
    This file contains an ADT for object pooling, with support of for range loops, iterators
    , reverse iterators, emplacement, swapping, subscript operator.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "SparseSet.h"

template <typename T, size_t N>
SparseSet<T, N>::SparseSet()
{
    for (size_t i = 0; i < N; ++i)
    {
        indexes[i] = i;
    }
}

template <typename T, size_t N>
SparseSet<T, N>::~SparseSet()
{
    for (T& element : *this)
    {
        element.~T();
    }
}

template <typename T, size_t N>
template <typename... Args>
T& SparseSet<T, N>::emplace_back(Args&&... args)
{
    T& back = *new (data + indexes[size_]) T(std::forward<Args>(args)...); // Construct the new element in the array
    ++size_;
    return back;
}


template <typename T, size_t N>
template <typename... Args>
T& SparseSet<T, N>::emplace_back(DenseIndex index, Args&&... args)
{
    //Find and make to next element
    T& back = *new (data + index.val) T(std::forward<Args>(args)...); // Construct the new element in the array
    for (size_t i = size_; i < N; ++i)
    {
        if (index[i] == index.val)
        {
            std::swap(index[i], index[size_]);
            break;
        }
    }
    ++size_;
    return back;
}

template <typename T, size_t N>
void SparseSet<T, N>::erase(size_t denseIndex)
{
    for (size_t i = 0; i < size_; ++i)
    {
        size_t& index = indexes[i];
        if (index == denseIndex)
        {
            reinterpret_cast<T*>(data)[index].~T();
            std::swap(index, indexes[size_ - 1]);
            --size_;
            return;
        }
    }
}

template <typename T, size_t N>
T& SparseSet<T, N>::operator[] (size_t i)
{
    //ASSERT(i >= size_, "ARRAY OUT OF BOUNDS");
    return *reinterpret_cast<T*>(data + indexes[i]);
}

template <typename T, size_t N>
T& SparseSet<T, N>::operator[] (DenseIndex index)
{
    return *reinterpret_cast<T*>(data + index.val);
}