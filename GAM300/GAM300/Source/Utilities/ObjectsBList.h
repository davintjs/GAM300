#ifndef OBJECTS_B_LIST_H
#define OBJECTS_B_LIST_H
#include "SparseSet.h"
#include <bitset>
#include "ObjectsList.h"

using ObjectIndex = size_t;

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
class ObjectsBList
{
    struct Node
    {
        SparseSet<ObjectsList<T,B_SZ>, N> sparseSetList;
        //Forward List
        Node* next = nullptr;
    };

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
        Iterator(size_t _index, size_t _subIndex, Node* _pNode) : index(_index), subIndex{ _subIndex },pNode{ _pNode } {}

        /***************************************************************************/
        /*!
            \brief
                Dereferencing operator to get value in it
            \return
                Reference to object stored
        */
        /**************************************************************************/
        T& operator*()
        {
            //for (auto& objectList : )
            return pNode->sparseSetList[index][subIndex];
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
            //Same type of objects that belong to the same entity
            //If it is full subIndex
            //Go to the next object set, aka sparseSetList
            ++subIndex;
            while(pNode && subIndex >= pNode->sparseSetList[index].size())
            {
                ++index;
                //If index is more than node size, go to next node
                while(pNode && index >= pNode->sparseSetList.size())
                {
                    index = 0;
                    pNode = pNode->next;
                }
                subIndex = 0;
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
            return pNode == other.pNode && index == other.index && subIndex == other.subIndex;
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
            return pNode != other.pNode || subIndex != other.subIndex || index != other.index;
        }


    };
public:
    template <typename... Args>
    T& emplace(DenseIndex index, Args&&... args);
    void clear();
    void erase(T& val);
    ~ObjectsBList();
    Iterator begin() 
    {
        Node* start = head;  
        while (start) 
        { 
            for (auto& val : start->sparseSetList)
            {
                if (!val.empty())
                    return Iterator(0, 0, start);
            }
            start = start->next;
        } 
        return Iterator(0, 0, nullptr); 
    }
    Iterator end() {return Iterator(0,0, nullptr); }

    size_t size() const { return size_; }

    T& DenseSubscript(DenseIndex val)
    {
        Node* start = head;
        while (val >= N)
        {
            val -= N;
            start = start->next;
        }
        return start->sparseSet.DenseSubscript(val);
    }

    DenseIndex GetDenseIndex(T& object)
    {
        Node* start = head;
        size_t i = 0;
        while (start)
        {
            for (auto& objectList : start->sparseSetList)
            {
                if (objectList.contains(object))
                {
                    return i + start->sparseSetList.GetDenseIndex(objectList);
                }
                //++i;
            }
            i += N;
            start = start->next;
        }
        ASSERT(true, "Object List does not contain this object");
    }
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
