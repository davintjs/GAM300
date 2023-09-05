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

#define OBJECTSBLIST ObjectsBList<T, N>
#define ITERATOR OBJECTSBLIST::Iterator

template <typename T, ObjectIndex N>
ITERATOR::Iterator(size_t _index, size_t _subIndex, Node* _pNode) : index(_index), subIndex{ _subIndex }, pNode{ _pNode } {}

template <typename T, ObjectIndex N>
T& ITERATOR::operator*()
{
	return pNode->sparseSetList[index][subIndex];
}

template <typename T, ObjectIndex N>
typename ITERATOR ITERATOR::operator++()
{
	++subIndex;
	while (pNode && subIndex >= pNode->sparseSetList[index].size())
	{
		++index;
		while (pNode && index >= pNode->sparseSetList.size())
		{
			index = 0;
			pNode = pNode->next;
		}
		subIndex = 0;
	}
	return *this;
}

template <typename T, ObjectIndex N>
typename ITERATOR ITERATOR::operator++(int)
{
	Iterator tmp(*this);
	operator++();
	return tmp;
}

template <typename T, ObjectIndex N>
bool ITERATOR::operator==(const Iterator& other) const
{
	return pNode == other.pNode && index == other.index && subIndex == other.subIndex;
}

template <typename T, ObjectIndex N>
bool ITERATOR::operator!=(const Iterator& other) const
{
	return pNode != other.pNode || subIndex != other.subIndex || index != other.index;
}

template <typename T, ObjectIndex N>
bool ITERATOR::IsActive()
{
	auto& objectList = pNode->sparseSetList[index];
	return objectList.IsActive(subIndex);
}

template <typename T, ObjectIndex N>
template <typename... Args>
T& OBJECTSBLIST::emplace(ObjectIndex denseIndex, Args&&... args)
{
	if (head == nullptr)
		head = tail = CreateNode();
	Node* start = head;
	while (denseIndex >= N)
	{
		if (start->next == nullptr)
			start->next = CreateNode();
		start = start->next;
		denseIndex -= N;
	}
	if (start->next == nullptr)
		tail = start;
	++size_;
	return start->sparseSetList.emplace(denseIndex).emplace_back();
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
		for (auto& subNode : start->sparseSetList)
		{
			if (subNode.contains(val))
			{
				subNode.erase(val);
				--size_;
				return;
			}
		}
		start = start->next;
	}
	ASSERT(start != nullptr, "Failed to erase value");
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
typename ITERATOR OBJECTSBLIST::begin()
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

template <typename T, ObjectIndex N>
typename ITERATOR OBJECTSBLIST::end() { return Iterator(0, 0, nullptr); }


template <typename T, ObjectIndex N>
size_t OBJECTSBLIST::size() const { return size_; }


template <typename T, ObjectIndex N>
void OBJECTSBLIST::SetActive(T& obj, bool val)
{
	Node* start = head;
	while (start)
	{
		for (auto it = start->sparseSetList.begin(); it != start->sparseSetList.end(); ++it)
		{
			if ((*it).contains(obj))
			{
				(*it).SetActive(obj, val);
				return;
			}
		}
		start = start->next;
	}
}


template <typename T, ObjectIndex N>
auto& OBJECTSBLIST::DenseSubscript(ObjectIndex val)
{
	Node* start = head;
	while (val >= N)
	{
		val -= N;
		start = start->next;
	}
	return start->sparseSetList.DenseSubscript(val);
}

template <typename T, ObjectIndex N>
ObjectIndex OBJECTSBLIST::GetDenseIndex(T& object)
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
		}
		i += N;
		start = start->next;
	}
	ASSERT(true, "Object List does not contain this object");
}