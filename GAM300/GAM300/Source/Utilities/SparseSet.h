/*!***************************************************************************************
\file			sparse-set.h
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
#include "Core/Debug.h"

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


        /***************************************************************************/
        /*!
            \brief
                Constructor for iterator
            \param _sparseIndex
                Index of element
            \param _arr
                Referenced sparse set
        */
        /**************************************************************************/
        Iterator(uint32_t _sparseIndex, SparseSet<T, N>& _arr);

        /***************************************************************************/
        /*!
            \brief
                Dereferencing operator to get value in it
            \return
                Reference to object stored
        */
        /**************************************************************************/
        T& operator*() const;

        /***************************************************************************/
         /*!
            \brief
                Gets the next iterator
            \return
                Next iterator
        */
        /**************************************************************************/
        Iterator operator++();

        /***************************************************************************/
        /*!
            \brief
                Increments this iterator and but returns the current iteration
            \return
                this iterator
        */
        /**************************************************************************/
        Iterator operator++(int);

        /***************************************************************************/
        /*!
            \brief
                Checks if two iterators are the same
            \param other
                Other iterator to compare with
            \return
                True if both iterators are of the same sparse index
        */
        /**************************************************************************/
        bool operator==(const Iterator& other) const;

        /***************************************************************************/
        /*!
            \brief
                Checks if two iterators are NOT the same
            \param other
                Other iterator to compare with
            \return
                True if both iterators are NOT of the same sparse index
        */
        /**************************************************************************/
        bool operator!=(const Iterator& other) const;
    };
    /***************************************************************************/
    /*!
        \brief
            Gets the starting iterator to this sparse set
        \return
            Starting iterator to this sparse set
    */
    /**************************************************************************/
    Iterator begin();

    /***************************************************************************/
    /*!
        \brief
            Gets the ending iterator to this sparse set
        \return
            Starting iterator to this sparse set
    */
    /**************************************************************************/
    Iterator end();

    /***************************************************************************/
    /*!
        \brief
            Default constructor, initializes all indexes in sequence.
    */
    /**************************************************************************/
    SparseSet();

    /***************************************************************************/
    /*!
        \brief
            Destructs all elements in the sparse set
    */
    /**************************************************************************/
    ~SparseSet();

    /***************************************************************************/
    /*!
        \brief
            Constructs a instance of the type by trying to call a constructor
            that takes in the same types of args
        \param args
            Args to be used to construct the object
        \return
            Constructed object
    */
    /**************************************************************************/
    template <typename... Args>
    T& emplace_back(Args&&... args);

    template <typename... Args>
    T& emplace(ObjectIndex index ,Args&&... args);
    /***************************************************************************/
    /*!
        \brief
            Erases from the sparse set by comparing memory addresses
        \param val
            Reference of value that would be used for memory address
    */
    /**************************************************************************/

    void erase(uint32_t denseIndex);


    void erase(T& val);

    /***************************************************************************/
    /*!
        \brief
            Gets the amount of objects in use
        \return
            Amount of objects in use
    */
    /**************************************************************************/
    uint32_t size() const;

    /***************************************************************************/
    /*!
        \brief
            Gets the amount of objects in use
        \param i
            Index of array
        \return
            Reference to object that was gotten through subscript
    */
    /**************************************************************************/
    T& operator[] (uint32_t i);

    T& DenseSubscript (ObjectIndex val);

    /***************************************************************************/
    /*!
        \brief
            Checks if object is part of this sparse set
        \param pValue
            Object to check
        \return
            True if the object is part of this sparse set
    */
    /**************************************************************************/
    bool contains(T& pValue);

    uint32_t GetDenseIndex(T& object);

    uint32_t GetDenseIndex(uint32_t sparseIndex);

    /***************************************************************************/
    /*!
        \brief
            Resets sparse set to size 0
    */
    /**************************************************************************/
    void clear();

    /***************************************************************************/
    /*!
        \brief
            Check if sparse set is empty
        \return
            True if no objects
    */
    /**************************************************************************/
    bool empty() const;

    bool full() const;

    T* TryGetDense(ObjectIndex denseIndex);

    bool TryErase(T& object);

    bool TryErase(ObjectIndex denseIndex);
};
#include "SparseSet.cpp"

#endif // !SPARSE_SET_H