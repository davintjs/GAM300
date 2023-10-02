/*!***************************************************************************************
\file			SparseSet.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
    This file contains the function declarations for a fixed size SparseSet that supports
    iterators and for range iteration.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef SPARSE_SET_H
#define SPARSE_SET_H

#include <initializer_list>
#include <array>
#include <iostream>

using ObjectIndex = uint32_t;

template <typename T, ObjectIndex N>
class SparseSet
{
    struct Node
    {
        ObjectIndex denseIndex;
        ObjectIndex sparseIndex;
    };
    //Uninitialized memory
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data[N];
    //DenseIndexes
    std::array<Node, N> nodes;
    uint32_t size_{ 0 };
public:

    class Iterator
    {
        SparseSet<T, N>& arr;
        uint32_t sparseIndex;
        friend class SparseSet;
    public:


        //Constructor for iterator
        Iterator(uint32_t _sparseIndex, SparseSet<T, N>& _arr);

        //Dereferencing operator to get value in it
        T& operator*() const;

        //Gets the next iterator
        Iterator operator++();

        //Increment operator
        Iterator operator++(int);

        //Compares two iterators
        bool operator==(const Iterator& other) const;

        //Compares checks if two iterators are not the same
        bool operator!=(const Iterator& other) const;
    };
    //Get the starting iterator
    Iterator begin();

    //Get the end iterator
    Iterator end();

    //Constructor
    SparseSet();

    //Destructor
    ~SparseSet();

    //Adds to sparseset by constructing object
    template <typename... Args>
    T& emplace_back(Args&&... args);

    //Adds to sparseset by constructing object at given position
    template <typename... Args>
    T& emplace(ObjectIndex index ,Args&&... args);

    //Erases element that matches denseIndex
    void erase(uint32_t denseIndex);

    //Erases element that matches val memory address
    void erase(T& val);

    //Amount of objects in the sparseSet
    uint32_t size() const;

    //Subscript operator
    T& operator[] (size_t i);

    //Dense subscript operator
    T& DenseSubscript (ObjectIndex val);

    //Check if object is within memory address of this array
    bool contains(T& pValue);

    //Get denseIndex of object
    uint32_t GetDenseIndex(T& object);

    //Get denseIndex of sparseIndex
    uint32_t GetDenseIndex(uint32_t sparseIndex);

    //Resets size to 0
    void clear();

    //Checks if sparseSet is empty
    bool empty() const;

    //Checks if sparseSet has hit its max capacity
    bool full() const;

    //Try to get a object by dense index
    T* TryGetDense(ObjectIndex denseIndex);

    //Try to erase a object by matching pointers
    bool TryErase(T& object);

    //Try to erase a object by dense index
    bool TryErase(ObjectIndex denseIndex);
};
#include "SparseSet.cpp"

#endif // !SPARSE_SET_H