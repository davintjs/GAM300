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

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
template <typename... Args>
T& ObjectsBList<T, N, B_SZ>::emplace(DenseIndex index, Args&&... args)
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
	while (!start->sparseSetList.full())
	{
		start->sparseSetList.emplace_back();
	}
	return start->sparseSetList.DenseSubscript(index).emplace_back();
}

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
void ObjectsBList<T, N, B_SZ>::clear()
{
	Node* start = head;
	while (start->sparseSetList.full())
	{
		start->sparseSetList.clear();
		start = start->next;
	}
}

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
typename ObjectsBList<T, N, B_SZ>::Node* ObjectsBList<T, N, B_SZ>::CreateNode()
{
	if (emptyNodesPool == nullptr)
		return new Node;
	Node* newNode = emptyNodesPool;
	newNode->next = nullptr;
	emptyNodesPool = emptyNodesPool->next;
	return newNode;
}

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
void ObjectsBList<T, N, B_SZ>::DeleteNode(Node* prev, Node* pNode)
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

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
void ObjectsBList<T, N, B_SZ>::erase(T& val)
{
	Node* prev = nullptr;
	Node* start = head;
	//Look for node/sparseset that contains the value
	while (!start->sparseSetList.contains(val))
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
		DeleteNode(prev, head);
}

template <typename T, ObjectIndex N, ObjectIndex B_SZ>
ObjectsBList<T, N, B_SZ>::~ObjectsBList()
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