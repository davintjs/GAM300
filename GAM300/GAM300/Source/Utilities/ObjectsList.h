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
        size_t index;
        friend class ObjectsList;
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
        Iterator(size_t _index, Node* _pNode);

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

        bool IsActive();
    };
public:
    template <typename... Args>
    T& emplace_back(Args&&... args);
    template <typename... Args>
    T& emplace(ObjectIndex index, Args&&... args);
    void clear();
    void erase(T& val);

    bool empty();

    bool IsActive(ObjectIndex sparseIndex);

    bool IsActiveDense(ObjectIndex denseIndex);

    void SetActive(ObjectIndex index, bool val = true);

    void SetActive(T& object, bool val = true);

    ~ObjectsList();
    Iterator begin();
    Iterator end();

    bool contains(T& val);

    T& operator[] (size_t i);

    size_t size() const;

    T& DenseSubscript(ObjectIndex val);


    ObjectIndex GetDenseIndex(T& object);

    ObjectIndex GetDenseIndex(ObjectIndex sparseIndex);
private:
    Node* head = nullptr;
    Node* tail = nullptr;
    Node* emptyNodesPool = nullptr;
    size_t size_{0};
    Node* CreateNode();
    void DeleteNode(Node* prev, Node* pNode);
};	

#include "ObjectsList.cpp"

#endif // !OBJECTS_LIST_H
