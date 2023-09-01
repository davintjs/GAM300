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
        nodes[i] = {i,i};
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
    T& back = *new (data + nodes[size_].denseIndex) T(std::forward<Args>(args)...); // Construct the new element in the array
    ++size_;
    return back;
}


template <typename T, size_t N>
template <typename... Args>
T& SparseSet<T, N>::emplace(DenseIndex index, Args&&... args)
{
    //Already exists
    if (nodes[index].sparseIndex < size_)
        return reinterpret_cast<T*>(data)[index];
    //Find and make to next element
    ASSERT(index < N, " OUT OF BOUNDS");
    T& back = *new (data + index) T(std::forward<Args>(args)...); // Construct the new element in the array
    for (size_t i = size_; i < N; ++i)
    {
        if (nodes[i].denseIndex == index)
        {
            size_t sparseIndex = nodes[i].sparseIndex;
            nodes[i].sparseIndex = nodes[size_].sparseIndex;
            nodes[size_].sparseIndex = sparseIndex;
            std::swap(nodes[i], nodes[size_]);
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
        Node& node = nodes[i];
        if (node.denseIndex == denseIndex)
        {
            reinterpret_cast<T*>(data)[node.denseIndex].~T();
            size_t sparseIndex = nodes[i].sparseIndex;
            nodes[i].sparseIndex = nodes[size_-1].sparseIndex;
            nodes[size_-1].sparseIndex = sparseIndex;
            std::swap(nodes[i], nodes[size_-1]);
            --size_;
            return;
        }
    }
}

template <typename T, size_t N>
T& SparseSet<T, N>::operator[] (size_t i)
{
    ASSERT(i < size_, "ARRAY OUT OF BOUNDS");
    return *reinterpret_cast<T*>(data + nodes[i].denseIndex);
}

template <typename T, size_t N>
T& SparseSet<T, N>::DenseSubscript(DenseIndex index)
{
    return reinterpret_cast<T*>(data)[index];
}