/*!***************************************************************************************
\file			ObjectsBList.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			02/09/2023

\brief
    This file defines a Object Bucket List manages a list of Objects Lists

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#ifndef OBJECTS_B_LIST_H
#define OBJECTS_B_LIST_H
#include "ObjectsList.h"

template <typename T, ObjectIndex N>
class ObjectsBList
{
    struct Node
    {
        ObjectsList<T,N> objectList;
        //Forward List
        Node* next = nullptr;
    };

public:
    class Iterator
    {
        Node* pNode;
        size_t index;
        friend class ObjectsBList;
    public:
        //Constructor for iterator
        Iterator(size_t _index, Node* _pNode);

        //Dereferencing operator to get value in it
        T& operator*();

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

    //Adds to array by constructing object at given position
    template <typename... Args>
    T& emplace(ObjectIndex index, Args&&... args);

    //Resets size to 0
    void clear();

    //Erases element that matches val memory address
    void erase(T& val);

    //Erases element that matches denseIndex
    void erase(ObjectIndex denseIndex);

    //Destructor
    ~ObjectsBList();
    
    //Get the starting iterator
    Iterator begin();

    //Get the end iterator
    Iterator end();

    //Amount of objects in the array
    size_t size() const;

    //Set an object to be active/inactive
    void SetActive(T& obj, bool val = true);

    //Dense subscript operator
    auto DenseSubscript(ObjectIndex val);
    
    //Gets dense index of an object
    ObjectIndex GetDenseIndex(T& object);
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

#include "ObjectsBList.cpp"

#endif // !OBJECTS_LIST_H
