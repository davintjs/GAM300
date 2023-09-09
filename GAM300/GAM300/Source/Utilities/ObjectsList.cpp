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

#include "ObjectsList.h"

#define OBJECTSLIST ObjectsList<T, N>
#define ITERATOR OBJECTSLIST::Iterator


template <typename T, ObjectIndex N>
ITERATOR::Iterator(size_t _index, Node * _pNode) : index(_index), pNode{_pNode} {}


template <typename T, ObjectIndex N>
T& ITERATOR::operator*() const
{
	return pNode->sparseSet[index];
}

template <typename T, ObjectIndex N>
typename ITERATOR ITERATOR::operator++()
{
	++index;
	while (pNode && index >= pNode->sparseSet.size())
	{
		index -= pNode->sparseSet.size();
		pNode = pNode->next;
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
	return pNode == other.pNode && index == other.index;
}

template <typename T, ObjectIndex N>
bool ITERATOR::operator!=(const Iterator& other) const
{
	return pNode != other.pNode || index != other.index;
}

template <typename T, ObjectIndex N>
bool ITERATOR::IsActive()
{
	return pNode->activeObjectsBitset.test(pNode->sparseSet.GetDenseIndex(index));
}

template <typename T, ObjectIndex N>
template <typename... Args>
T& OBJECTSLIST::emplace_back(Args&&... args)
{
	if (head == nullptr)
		head = tail = CreateNode();
	Node* start = head;
	while (start->next)
	{
		start = start->next;
	}
	if (start->sparseSet.full())
	{
		tail = start->next = CreateNode();
		start = start->next;
	}
	++size_;
	return start->sparseSet.emplace_back(args...);
}

template <typename T, ObjectIndex N>
template <typename... Args>
T& OBJECTSLIST::emplace(ObjectIndex index, Args&&... args)
{
	if (head == nullptr)
		head = tail = CreateNode();
	Node* start = head;
	while (index >= N)
	{
		if (start->next == nullptr)
			start->next = CreateNode();
		start = start->next;
		index -= N;
	}
	if (start->next == nullptr)
		tail = start;
	++size_;
	return start->sparseSet.emplace(index);
}

template <typename T, ObjectIndex N>
void OBJECTSLIST::clear()
{
	Node* start = head;
	while (start)
	{
		Node* prev = start;
		start->sparseSet.clear();
		start = start->next;
	}
}

template <typename T, ObjectIndex N>
typename OBJECTSLIST::Node* OBJECTSLIST::CreateNode()
{
	if (emptyNodesPool == nullptr)
		return new Node;
	Node* newNode = emptyNodesPool;
	newNode->next = nullptr;
	emptyNodesPool = emptyNodesPool->next;
	return newNode;
}

template <typename T, ObjectIndex N>
void OBJECTSLIST::DeleteNode(Node* prev, Node* pNode)
{
	if (prev)
	{
		prev->next = pNode->next;
	}

	if (head == pNode)
		head = pNode->next;
	//If tail node being deleted
	if (tail == pNode)
		tail = prev;

	pNode->next = emptyNodesPool;
	emptyNodesPool = pNode;
}


template <typename T, ObjectIndex N>
bool OBJECTSLIST::TryErase(T& val)
{
	Node* start = head;
	while (start)
	{
		if (start->sparseSet.TryErase(val))
		{
			--size_;
			return true;
		}
		start = start->next;
	}
	return false;
}

template <typename T, ObjectIndex N>
bool OBJECTSLIST::TryErase(ObjectIndex denseIndex)
{
	Node* start = head;
	while (start)
	{
		if (start->sparseSet.TryErase(denseIndex))
		{
			--size_;
			return true;
		}
		start = start->next;
		denseIndex -= N;
	}
	return false;
}

template <typename T, ObjectIndex N>
void OBJECTSLIST::erase(T& val)
{
	Node* start = head;
	//Look for node/sparseset that contains the value
	while (!start->sparseSet.contains(val))
	{
		start = start->next;
	}
	E_ASSERT(start != nullptr, "Failed to erase value");
	start->sparseSet.erase(val);
	--size_;
}

template <typename T, ObjectIndex N>
OBJECTSLIST::~ObjectsList()
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
bool OBJECTSLIST::empty()
{
	return size_ == 0;
}

template <typename T, ObjectIndex N>
bool OBJECTSLIST::IsActive(ObjectIndex sparseIndex)
{
	Node* start = head;
	while (start && sparseIndex >= N)
	{
		sparseIndex -= start->sparseSet.size();
		start = start->next;
	}
	return start->activeObjectsBitset.test(start->sparseSet.GetDenseIndex(sparseIndex));
}

template <typename T, ObjectIndex N>
bool OBJECTSLIST::IsActiveDense(ObjectIndex denseIndex)
{
	Node* start = head;
	while (start && denseIndex >= N)
	{
		denseIndex -= N;
		start = start->next;
	}
	return start->activeObjectsBitset.test(denseIndex);
}

template <typename T, ObjectIndex N>
void OBJECTSLIST::SetActive(ObjectIndex index, bool val)
{
	Node* start = head;
	while (start && index >= N)
	{
		index -= N;
		start = start->next;
	}
	start->activeObjectsBitset.set(index, val);
}

template <typename T, ObjectIndex N>
void OBJECTSLIST::SetActive(T& object, bool val)
{
	SetActive(GetDenseIndex(object), val);
}

template <typename T, ObjectIndex N>
bool OBJECTSLIST::contains(T& val)
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

template <typename T, ObjectIndex N>
T& OBJECTSLIST::operator[] (size_t i)
{
	Node* start = head;
	while (i >= start->sparseSet.size())
	{
		i -= start->sparseSet.size();
		start = start->next;
	}
	return start->sparseSet[i];
}

template <typename T, ObjectIndex N>
size_t OBJECTSLIST::size() const { return size_; }

template <typename T, ObjectIndex N>
T& OBJECTSLIST::DenseSubscript(ObjectIndex val)
{
	Node* start = head;
	while (val >= N)
	{
		val -= N;
		start = start->next;
	}
	return start->sparseSet.DenseSubscript(val);
}

template <typename T, ObjectIndex N>
ObjectIndex OBJECTSLIST::GetDenseIndex(T& object)
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
	E_ASSERT(true, "Object List does not contain this object");
}

template <typename T, ObjectIndex N>
ObjectIndex OBJECTSLIST::GetDenseIndex(ObjectIndex sparseIndex)
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

template <typename T, ObjectIndex N>
T* OBJECTSLIST::TryGetDense(ObjectIndex denseIndex)
{
	Node* start = head;
	while (start && denseIndex >= N)
	{
		denseIndex -= N;
		start = start->next;
	}
	if (start == nullptr)
		return nullptr;
	return start->sparseSet.TryGetDense(denseIndex);
}

template <typename T, ObjectIndex N>
bool OBJECTSLIST::TrySetActive(ObjectIndex denseIndex, bool val)
{
	Node* start = head;
	while (start && denseIndex >= start->sparseSet.size())
	{
		//Failed to find
		if (denseIndex < N)
			break;
		denseIndex -= N;
		start = start->next;
	}
	if (start == nullptr)
		return false;
	start->activeObjectsBitset.set(denseIndex, val);
	return true;
}

template <typename T, ObjectIndex N>
bool OBJECTSLIST::TrySetActive(T& object, bool val)
{
	Node* start = head;
	while (start)
	{
		//Found
		if (start->sparseSet.contains(object))
			break;
		start = start->next;
	}
	if (start == nullptr)
		return false;
	ObjectIndex denseIndex = start->sparseSet.GetDenseIndex(object);
	start->activeObjectsBitset.set(denseIndex, val);
	return true;
}

template <typename T, ObjectIndex N>
typename ITERATOR OBJECTSLIST::begin() { Node* start = head;  while (start && start->sparseSet.empty()) start = start->next; return Iterator(0, start); }

template <typename T, ObjectIndex N>
typename ITERATOR OBJECTSLIST::end() { return Iterator(0, nullptr); }