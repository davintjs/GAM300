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
		head = new Node;
	Node* start = head;
	while (start->sparseSet.full())
	{
		start = start->next;
	}
	return start->sparseSet.emplace_back(args...);
}

template <typename T, size_t N>
template <typename... Args>
T& ObjectsList<T, N>::emplace_back(DenseIndex index, Args&&... args)
{
	//Find and make to next element
	T& back = *new (data + index.val) T(std::forward<Args>(args)...); // Construct the new element in the array
	for (size_t i = size_; i < N; ++i)
	{
		if (index[i] == index.val)
		{
			std::swap(index[i], index[size_]);
			break;
		}
	}
	++size_;
	return back;
}

template <typename T, ObjectIndex N>
void ObjectsList<T, N>::clear()
{
	Node* start = head;
	while (start->sparseSet.full())
	{
		start->sparseSet.clear();
		start = start->next;
	}
}

template <typename T, ObjectIndex N>
typename ObjectsList<T, N>::Node* ObjectsList<T, N>::CreateNode()
{
	if (emptyNodesPool != nullptr)
	{
		return new Node;
	}
	Node* newNode = emptyNodesPool;
	emptyNodesPool = emptyNodesPool->next;
	return newNode;
}

template <typename T, ObjectIndex N>
void ObjectsList<T, N>::DeleteNode(Node* prev, Node* pNode)
{
	if (prev)
		prev->next = pNode->next;
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
	if (start != nullptr)
	{
		SparseSet<T, N>& sparseSet = start->sparseSet;
		sparseSet.erase(val);
		--size_;
		if (sparseSet.empty())
			DeleteNode(prev, head);
	}
	else
		std::cout << "Failed to erase value" << std::endl;
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