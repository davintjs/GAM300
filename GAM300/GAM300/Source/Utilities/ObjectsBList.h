#ifndef OBJECTS_B_LIST_H
#define OBJECTS_B_LIST_H
#include "SparseSet.h"
#include <bitset>
#include "ObjectsList.h"

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
class ObjectsBList
{
    struct Node
    {
        SparseSet<ObjectsList<T,B_SZ>, N> sparseSetList;
        //Forward List
        Node* next = nullptr;
    };

public:
    class Iterator
    {
        Node* pNode;
        size_t index;
        size_t subIndex;
        friend class ObjectsBList;
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
        Iterator(size_t _index, size_t _subIndex, Node* _pNode);

        /***************************************************************************/
        /*!
            \brief
                Dereferencing operator to get value in it
            \return
                Reference to object stored
        */
        /**************************************************************************/
        T& operator*();

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
        bool IsActive();
    };
    template <typename... Args>
    T& emplace(ObjectIndex index, Args&&... args);
    void clear();
    void erase(T& val);
    ~ObjectsBList();
    Iterator begin();
    Iterator end();

    size_t size() const;

    void SetActive(T& obj, bool val = true);

    auto& DenseSubscript(ObjectIndex val);

    ObjectsList<T, B_SZ>& DenseArray(ObjectIndex val);

    ObjectIndex GetDenseIndex(T& object);
private:
    Node* head = nullptr;
    Node* tail = nullptr;
    Node* emptyNodesPool = nullptr;
    size_t size_{0};
    Node* CreateNode();
    void DeleteNode(Node* prev, Node* pNode);
};	

#include "ObjectsBList.cpp"

#endif // !OBJECTS_LIST_H
