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
        Iterator(size_t _index, Node* _pNode) : index(_index), pNode{ _pNode } {}

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
            return pNode->sparseSet[index];
        }

        /***************************************************************************/
         /*!
            \brief
                Gets the next iterator
            \return
                Next iterator
        */
        /**************************************************************************/
        Iterator operator++() 
        {
            ++index;
            while (pNode && index >= pNode->sparseSet.size())
            {
                index -= pNode->sparseSet.size();
                pNode = pNode->next;
            }
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
            return pNode == other.pNode && index == other.index;
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
            return pNode != other.pNode || index != other.index;
        }

        bool IsActive()
        {
            return pNode->activeObjectsBitset.test(pNode->sparseSet.GetDenseIndex(index));
        }
    };
public:
    template <typename... Args>
    T& emplace_back(Args&&... args);
    template <typename... Args>
    T& emplace(ObjectIndex index, Args&&... args);
    void clear();
    void erase(T& val);

    bool empty()
    {
        return size_ == 0;
    }

    bool IsActive(ObjectIndex sparseIndex)
    {
        Node* start = head;
        while (start && sparseIndex >= N)
        {
            sparseIndex -= N;
            start = start->next;
        }
        return start->activeObjectsBitset.test(start->sparseSet.GetDenseIndex(sparseIndex));
    }

    void SetActive(ObjectIndex index, bool val = true)
    {
        Node* start = head;
        while (start && index >= N)
        {
            index -= N;
            start = start->next;
        }
        start->activeObjectsBitset.set(index,val);
    }

    void SetActive(T& object, bool val = true)
    {
        SetActive(GetDenseIndex(object),val);
    }

    ~ObjectsList();
    Iterator begin() { Node* start = head;  while (start && start->sparseSet.empty()) start = start->next; return Iterator(0, start); }
    Iterator end() {return Iterator(0, nullptr);}

    bool contains(T& val)
    {
        Node* start = head;
        while (start)
        {
            if (start->sparseSet.contains(val))
                return true;
            start = start->next;
        }
        return false;
    }

    T& operator[] (size_t i)
    {
        Node* start = head;
        while (i >= start->sparseSet.size())
        {
            i -= start->sparseSet.size();
            start = start->next;
        }
        return start->sparseSet[i];
    }

    size_t size() const { return size_; }

    T& DenseSubscript(ObjectIndex val)
    {
        Node* start = head;
        while (val >= N)
        {
            val -= N;
            start = start->next;
        }
        return start->sparseSet.DenseSubscript(val);
    }

    ObjectIndex GetDenseIndex(T& object)
    {
        Node* start = head;
        ObjectIndex count = 0;
        while (start != nullptr)
        {
            if (start->sparseSet.contains(object))
                return start->sparseSet.GetDenseIndex(object) + count * N;
            ++count;
            start = start->next;
        }
        ASSERT(true, "Object List does not contain this object");
    }

    ObjectIndex GetDenseIndex(ObjectIndex sparseIndex)
    {
        Node* start = head;
        ObjectIndex index = 0;
        while (start && sparseIndex >= start->sparseSet.size())
        {
            index += N;
            sparseIndex -= start->sparseSet.size();
            start = start->next;
        }
        return index + start->sparseSet.GetDenseIndex(sparseIndex);
    }
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
