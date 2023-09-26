/*!***************************************************************************************
\file			ObjectsList.h
\project
\author			Zacharie Hong

\par			Course: GAM250
\par			Section:
\date			27/06/2023

\brief

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "ObjectsBList.h"
#include <vector>

#define OBJECTSBLIST ObjectsBList<T, N>
#define OBL_ITER OBJECTSBLIST::Iterator

template <typename T, ObjectIndex N>
OBL_ITER::Iterator(size_t _index, Node* _pNode) : index(_index), pNode{ _pNode } {}

template <typename T, ObjectIndex N>
T& OBL_ITER::operator*()
{
	return pNode->objectList[index];
}

template <typename T, ObjectIndex N>
typename OBL_ITER OBL_ITER::operator++()
{
	++index;
	if (index >= pNode->objectList.size())
	{
		do
		{
			pNode = pNode->next;
		} 
		while (pNode && pNode->objectList.empty());
		index = 0;
	}
	return *this;
}

template <typename T, ObjectIndex N>
typename OBL_ITER OBL_ITER::operator++(int)
{
	Iterator tmp(*this);
	operator++();
	return tmp;
}

template <typename T, ObjectIndex N>
bool OBL_ITER::operator==(const Iterator& other) const
{
	return pNode == other.pNode && index == other.index;
}

template <typename T, ObjectIndex N>
bool OBL_ITER::operator!=(const Iterator& other) const
{
	return pNode != other.pNode || index != other.index;
}

template <typename T, ObjectIndex N>
bool OBL_ITER::IsActive()
{
	return pNode->objectList.IsActive(index);
}

template <typename T, ObjectIndex N>
template <typename... Args>
T& OBJECTSBLIST::emplace(ObjectIndex denseIndex, Args&&... args)
{
	if (head == nullptr)
		head = tail = CreateNode();
	Node* start = head;
	while (start->objectList.TryGetDense(denseIndex) != nullptr)
	{
		if (start->next == nullptr)
		{
			start->next = CreateNode();
			start = start->next;
			tail = start;
			break;
		}
		start = start->next;
	}
	++size_;
	return start->objectList.emplace(denseIndex);
}

template <typename T, ObjectIndex N>
void OBJECTSBLIST::clear()
{
	Node* start = head;
	while (start->sparseSetList.full())
	{
		start->sparseSetList.clear();
		start = start->next;
	}
}

template <typename T, ObjectIndex N>
typename OBJECTSBLIST::Node* ObjectsBList<T, N>::CreateNode()
{
	if (emptyNodesPool == nullptr)
		return new Node;
	Node* newNode = emptyNodesPool;
	newNode->next = nullptr;
	emptyNodesPool = emptyNodesPool->next;
	return newNode;
}

template <typename T, ObjectIndex N>
void OBJECTSBLIST::DeleteNode(Node* prev, Node* pNode)
{
	if (prev)
	{
		prev->next = pNode->next;
	}

	//If tail node being deleted
	if (tail == pNode)
		tail = prev;

	if (pNode == head)
		tail = nullptr;
	pNode->next = emptyNodesPool;
	emptyNodesPool = pNode;
}

template <typename T, ObjectIndex N>
void OBJECTSBLIST::erase(T& val)
{
	Node* start = head;
	//Look for node/sparseset that contains the value
	while (start)
	{
		if (start->objectList.TryErase(val))
			return;
		start = start->next;
	}
	E_ASSERT(start != nullptr, "Failed to erase value");
}

template <typename T, ObjectIndex N>
void OBJECTSBLIST::erase(ObjectIndex denseIndex)
{
	Node* start{ head };
	while (start)
	{
		if (start->objectList.TryErase(denseIndex))
			--size_;
		start = start->next;
	}
}

template <typename T, ObjectIndex N>
OBJECTSBLIST::~ObjectsBList()
{
	Node* start = head;
	while (start)
	{
		Node* node = start;
		start = start->next;
		delete node;
	}

	Node* emptyStart = emptyNodesPool;
	while (emptyStart)
	{
		Node* node = emptyStart;
		emptyStart = emptyStart->next;
		delete node;
	}
}

template <typename T, ObjectIndex N>
typename OBL_ITER OBJECTSBLIST::begin()
{
	Node* start = head;
	while (start && start->objectList.empty())
	{
		start = start->next;
	}
	return Iterator(0, start);
}

template <typename T, ObjectIndex N>
typename OBL_ITER OBJECTSBLIST::end() { return Iterator(0, nullptr); }


template <typename T, ObjectIndex N>
size_t OBJECTSBLIST::size() const { return size_; }


template <typename T, ObjectIndex N>
void OBJECTSBLIST::SetActive(T& obj, bool val)
{
	Node* start = head;
	while (start)
	{
		if (start->objectList.TrySetActive(obj, val))
			return;
		start = start->next;
	}
	E_ASSERT(false,"FAILED TO SET ACTIVE");
}

template <typename T, ObjectIndex N>
auto OBJECTSBLIST::DenseSubscript(ObjectIndex denseIndex)
{
	std::vector<T*> references;
	Node* start = head;
	while (start)
	{
		T* pObject = start->objectList.TryGetDense(denseIndex);
		if (pObject)
			references.push_back(pObject);
		start = start->next;
	}
	return std::move(references);
}

template <typename T, ObjectIndex N>
ObjectIndex OBJECTSBLIST::GetDenseIndex(T& object)
{
	Node* start = head;
	while (start)
	{
		if (start->objectList.contains(object))
		{
			return start->objectList.GetDenseIndex(object);
		}
		start = start->next;
	}
	E_ASSERT(true, "Object List does not contain this object");
}