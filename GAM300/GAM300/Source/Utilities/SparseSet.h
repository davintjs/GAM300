/*!***************************************************************************************
\file			sparse-set.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			10/03/2023

\brief
    This file contains an ADT for object pooling, with support of for range loops, iterators
    , reverse iterators, emplacement, swapping, subscript operator.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef SPARSE_SET_H
#define SPARSE_SET_H

#include <initializer_list>
#include <array>
#include <iostream>
#include "Core/Debug.h"

template <typename T, size_t N>
class SparseSet
{
    //Uninitialized memory
    typename std::aligned_storage<sizeof(T), alignof(T)>::type data[N];
    std::array<size_t, N>indexes;
    size_t size_{ 0 };
public:
    class Iterator
    {

        SparseSet<T, N>& arr;
        size_t sparseIndex;
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
        Iterator(size_t _sparseIndex, SparseSet<T, N>& _arr) : sparseIndex(_sparseIndex), arr{ _arr } {}

        /***************************************************************************/
        /*!
            \brief
                Dereferencing operator to get value in it
            \return
                Reference to object stored
        */
        /**************************************************************************/
        T& operator*() const
        {
            return arr[sparseIndex];
        }

        /***************************************************************************/
         /*!
            \brief
                Gets the next iterator
            \return
                Next iterator
        */
        /**************************************************************************/
        Iterator operator++() {
            ++sparseIndex;
            return *this;
        }

        /***************************************************************************/
        /*!
            \brief
                Increments this iterator and but returns the current iteration
            \return
                this iterator
        */
        /**************************************************************************/
        Iterator operator++(int) {
            Iterator tmp(*this);
            operator++();
            return tmp;
        }

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
        bool operator==(const Iterator& other) const {
            return sparseIndex == other.sparseIndex;
        }

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
        bool operator!=(const Iterator& other) const {
            return sparseIndex != other.sparseIndex;
        }
    };
    class ReverseIterator
    {
        SparseSet<T, N>& arr;
        int sparseIndex;
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
        ReverseIterator(size_t _sparseIndex, SparseSet<T, N>& _arr) : sparseIndex(_sparseIndex), arr{ _arr } {}
        /***************************************************************************/
        /*!
            \brief
                Dereferencing operator to get value in it
            \return
                Reference to object stored
        */
        /**************************************************************************/
        T& operator*() const
        {
            return arr[sparseIndex];
        }
        /***************************************************************************/
         /*!
            \brief
                Gets the next iterator
            \return
                Next iterator
        */
        /**************************************************************************/
        ReverseIterator operator++() {
            --sparseIndex;
            return *this;
        }
        /***************************************************************************/
        /*!
            \brief
                Increments this iterator and but returns the current iteration
            \return
                this iterator
        */
        /**************************************************************************/
        ReverseIterator operator++(int) {
            ReverseIterator tmp(*this);
            operator++();
            return tmp;
        }
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
        bool operator==(const ReverseIterator& other) const {
            return sparseIndex == other.sparseIndex;
        }
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
        bool operator!=(const ReverseIterator& other) const {
            return sparseIndex != other.sparseIndex;
        }
    };

    /***************************************************************************/
    /*!
        \brief
            Gets the starting iterator to this sparse set
        \return
            Starting iterator to this sparse set
    */
    /**************************************************************************/
    Iterator begin() {
        return Iterator(0, *this);
    }

    /***************************************************************************/
    /*!
        \brief
            Gets the ending iterator to this sparse set
        \return
            Starting iterator to this sparse set
    */
    /**************************************************************************/
    Iterator end() {
        return Iterator(size_, *this);
    }

    /***************************************************************************/
    /*!
        \brief
            Gets the reverse starting iterator to this sparse set
        \return
            Reverse starting iterator to this sparse set
    */
    /**************************************************************************/
    ReverseIterator rbegin() {
        return ReverseIterator(size_ - 1, *this);
    }

    /***************************************************************************/
    /*!
        \brief
            Gets the reverse end iterator to this sparse set
        \return
            Reverse end iterator to this sparse set
    */
    /**************************************************************************/
    ReverseIterator rend() {
        return ReverseIterator(-1, *this);
    }

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
    ~SparseSet()
    {
        for (T& element : *this)
        {
            element.~T();
        }
        //PRINT("SPARSE SET DECONSTRUCTOR ");
    }

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
    T& emplace_back(Args&&... args)
    {
        T& back = *new (data + indexes[size_]) T(std::forward<Args>(args)...); // Construct the new element in the array
        ++size_;
        return back;
    }

    /***************************************************************************/
    /*!
        \brief
            Erases from the sparse set by comparing memory addresses
        \param val
            Reference of value that would be used for memory address
    */
    /**************************************************************************/
    void erase(T& val)
    {
        //Find index first
        for (size_t i = 0; i < size_; ++i)
        {
            if (reinterpret_cast<T*>(data + indexes[i]) == &val)
            {
                size_t index = indexes[i];

                reinterpret_cast<T*>(data)[index].~T();

                std::ignore = std::remove(indexes.begin(), indexes.begin() + size_, index);
                indexes[size_ - 1] = index;
                --size_;
                return;
            }
        }
        //ASSERT(true, "Value is not an element of this array");
    }

    /***************************************************************************/
    /*!
        \brief
            Erases from the sparse set by comparing iterators
        \param iter
            Iterator to match to remove the element
    */
    /**************************************************************************/
    void erase(const Iterator& iter)
    {
        //ASSERT(size_ == 0, "Can't erase from empty array");
        size_t index = indexes[iter.sparseIndex];
        reinterpret_cast<T*>(data)[index].~T();
        std::ignore = std::remove(indexes.begin(), indexes.begin() + size_, index);
        indexes[size_ - 1] = index;
        --size_;
    }

    /***************************************************************************/
    /*!
        \brief
            Gets the amount of objects in use
        \return
            Amount of objects in use
    */
    /**************************************************************************/
    size_t size() const
    {
        return size_;
    }

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
    T& operator[] (size_t i);

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
    bool exists(T* pValue)
    {
        size_t denseIndex = pValue - static_cast<T*>(data);
        for (size_t i = 0; i < size_; ++i)
        {
            if (indexes[i] == denseIndex)
            {
                return true;
            }
        }
        return false;
    }

    /***************************************************************************/
    /*!
        \brief
            Swaps two objects based on their sparse indexes
        \param sparseIndex1
            Index of first object to swap
        \param sparseIndex2
            Index of second object to swap
    */
    /**************************************************************************/
    void swap(size_t sparseIndex1, size_t sparseIndex2)
    {
        size_t tmp{ indexes[sparseIndex1] };
        indexes[sparseIndex1] = indexes[sparseIndex2];
        indexes[sparseIndex2] = tmp;
    }

    /***************************************************************************/
    /*!
        \brief
            Swaps two objects based on their memory location
        \param lhs
            First object to swap
        \param rhs
            Second object to swap
    */
    /**************************************************************************/
    void swap(T& lhs, T& rhs)
    {
        size_t rhsDenseIndex = &rhs - reinterpret_cast<T*>(data);
        size_t lhsDenseIndex = &lhs - reinterpret_cast<T*>(data);
        //ASSERT(rhsDenseIndex >= size_, "RHS is not an element of this array");
        //ASSERT(lhsDenseIndex >= size_, "LHS is not an element of this array");
        if (lhsDenseIndex > rhsDenseIndex)
        {
            for (size_t i = 0; i < size_; ++i)
            {
                size_t& index = indexes[i];
                if (index == rhsDenseIndex)
                    index = lhsDenseIndex;
                else if (index == lhsDenseIndex)
                {
                    index = rhsDenseIndex;
                    return;
                }
            }
        }
        else if (lhsDenseIndex < rhsDenseIndex)
        {
            for (size_t i = 0; i < size_; ++i)
            {
                size_t& index = indexes[i];
                if (index == lhsDenseIndex)
                    index = rhsDenseIndex;
                else if (index == rhsDenseIndex)
                {
                    index = lhsDenseIndex;
                    return;
                }
            }
        }
    }

    /***************************************************************************/
    /*!
        \brief
            Resets sparse set to size 0
    */
    /**************************************************************************/
    void clear() { size_ = 0; }

    /***************************************************************************/
    /*!
        \brief
            Check if sparse set is empty
        \return
            True if no objects
    */
    /**************************************************************************/
    bool empty() const { return !size_; }

    template <typename T, size_t N>
    friend std::ostream& operator<<(std::ostream& stream, SparseSet<T, N>& sS);
};

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& stream, SparseSet<T, N>& sS)
{
    for (size_t i{ 0 }; i < sS.size_; ++i)
    {
        stream << sS.indexes[i] << ",";
    }
    return stream;
}

template <typename T, size_t N>
SparseSet<T, N>::SparseSet()
{
    for (size_t i = 0; i < N; ++i)
    {
        indexes[i] = i;
    }
    //PRINT("SPARSE SET CONSTRUCTED");
}

template <typename T, size_t N>
T& SparseSet<T, N>::operator[] (size_t i)
{
    //ASSERT(i >= size_, "ARRAY OUT OF BOUNDS");
    return *reinterpret_cast<T*>(data + indexes[i]);
}

#endif // !SPARSE_SET_H