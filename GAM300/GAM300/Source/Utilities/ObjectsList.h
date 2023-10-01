/*!***************************************************************************************
\file			ObjectsList.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
    This file declares a Object List which manages sparse sets

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef OBJECTS_LIST_H
#define OBJECTS_LIST_H
#include "SparseSet.h"
#include <bitset>

template <typename T, ObjectIndex N>
class ObjectsList
{
    struct Node
    {
        SparseSet<T, N> sparseSet;
        std::bitset<N> activeObjectsBitset;
        //Forward List
        Node* next = nullptr;
    };

    class Iterator
    {
        Node* pNode;
        ObjectIndex index;
        friend class ObjectsList;
    public:
        //Constructor for iterator
        Iterator(ObjectIndex _index, Node* _pNode);

        //Dereferencing operator to get value in it
        T& operator*() const;

        //Gets the next iterator
        Iterator operator++();

        //Increments this iterator and but returns the current iteration
        Iterator operator++(int);

        //Checks if two iterators are the same
        bool operator==(const Iterator& other) const;

        //Checks if two iterators are NOT the same
        bool operator!=(const Iterator& other) const;

        //Check if the object in the iterator is active
        bool IsActive();
    };
public:
    //Adds to th array by constructing object at the back
    template <typename... Args>
    T& emplace_back(Args&&... args);

    //Adds to array by constructing object at given position
    template <typename... Args>
    T& emplace(ObjectIndex index, Args&&... args);

    //Resets size to 0
    void clear();

    //Erases element that matches val memory address
    void erase(T& val);

    //Try to erase a element that matches val memory address
    bool TryErase(T& val);

    //Try to erase a element that matches dense index
    bool TryErase(ObjectIndex denseIndex);

    //Checks if array is empty
    bool empty();

    //Checks if something is active at given sparse index
    bool IsActive(ObjectIndex sparseIndex);

    //Checks if something is active at given dense index
    bool IsActiveDense(ObjectIndex denseIndex);

    //Set an object at given index to be active/inactive
    void SetActive(ObjectIndex index, bool val = true);

    //Set an object to be active/inactive
    void SetActive(T& object, bool val = true);

    //Destructor
    ~ObjectsList();

    //Get the starting iterator
    Iterator begin();

    //Get the end iterator
    Iterator end();

    //Check if this should contain a value
    bool contains(T& val);

    //Subscript operator
    T& operator[] (size_t i);

    //Amount of objects in the array
    size_t size() const;

    //Dense subscript operator
    T& DenseSubscript(ObjectIndex val);

    //Gets dense index of an object
    ObjectIndex GetDenseIndex(T& object);

    //Gets dense index of a sparseIndex
    ObjectIndex GetDenseIndex(ObjectIndex sparseIndex);

    //Try to get the object at given dense index
    T* TryGetDense(ObjectIndex denseIndex);

    //Try to set the object at given index to active or inactive
    bool TrySetActive(ObjectIndex denseIndex, bool val = true);

    //Try to set the object to active or inactive
    bool TrySetActive(T& object, bool val = true);

private:
    Node* head = nullptr;
    Node* tail = nullptr;
    Node* emptyNodesPool = nullptr;
    size_t size_{0};

    //Creates a new node
    Node* CreateNode();

    //Deletes a node
    void DeleteNode(Node* prev, Node* pNode);
};	

#include "ObjectsList.cpp"

#endif // !OBJECTS_LIST_H
