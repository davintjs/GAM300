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


template <typename T, ObjectIndex N>
template <typename... Args>
T& ObjectsList<T, N>::emplace_back(Args&&... args)
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
T& ObjectsList<T, N>::emplace(DenseIndex index, Args&&... args)
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
void ObjectsList<T, N>::clear()
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
typename ObjectsList<T, N>::Node* ObjectsList<T, N>::CreateNode()
{
	if (emptyNodesPool == nullptr)
		return new Node;
	Node* newNode = emptyNodesPool;
	newNode->next = nullptr;
	emptyNodesPool = emptyNodesPool->next;
	return newNode;
}

template <typename T, ObjectIndex N>
void ObjectsList<T, N>::DeleteNode(Node* prev, Node* pNode)
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
void ObjectsList<T, N>::erase(T& val)
{
	Node* prev = nullptr;
	Node* start = head;
	//Look for node/sparseset that contains the value
	while (!start->sparseSet.contains(val))
	{
		prev = start;
		start = start->next;
	}
	ASSERT(start != nullptr, "Failed to erase value");
	SparseSet<T, N>& sparseSet = start->sparseSet;
	sparseSet.erase(sparseSet.GetDenseIndex(val));
	--size_;
	//Delete node only if its the last one
	if (sparseSet.empty() && start->next == nullptr)
		DeleteNode(prev, start);
}

template <typename T, ObjectIndex N>
ObjectsList<T, N>::~ObjectsList()
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