/*!***************************************************************************************
\file			Pathfinder.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           19/11/2023

\brief
	This file contains the declarations of the following:
	1. OnList enum class
		a. Gives us information on a PathNode current state in the priority queue
	2. PathNode class
		a. A node of the navmesh generated for AI to find its waypoint
	3. AStarPather class
		a. A* pathfinding for AI to find its waypoint to goal with portaling and funneling
		b. Calculates heuristic using euclidean method
		c. Helper functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include <vector>
#include <deque>
#include <queue>
#include <glm/glm.hpp>

class Triangle3D;
extern bool CompareTriangles(Triangle3D* lhs, Triangle3D* rhs);

enum class OnList
{
	NONE,
	OPEN_LIST,
	CLOSED_LIST
};

class PathNode
{
public:
	PathNode() = default;
	~PathNode() = default;

protected:
	Triangle3D* mParent = nullptr;
	float mFinalCost = 0.f;
	float mHeuCost = 0.f;
	float mGivenCost = 0.f;
	OnList mOnList = OnList::NONE;
};

class OpenList
{
public:
	void push(Triangle3D* n);
	Triangle3D* pop();
	std::vector<Triangle3D*> nodes;
};

class AStarPather
{
public:
	bool ComputePath(Triangle3D* mStart, Triangle3D* mEnd);
	std::deque<glm::vec3> PathPostProcess(const glm::vec3& mStart, const glm::vec3& mEnd);
	void ResetPather();

private:
	float CalculateHeuristic(const Triangle3D* mCurrNode, const Triangle3D* mEnd);

	// Post-processing
	std::vector<std::pair<glm::vec3, glm::vec3>> GetPortals(const glm::vec3& mStart, const glm::vec3& mEnd);
	std::deque<glm::vec3> Funnel(const glm::vec3& mStart, const glm::vec3& mEnd, const std::vector<std::pair<glm::vec3, glm::vec3>>& mPortals);

	float IsAPointLeftOfVectorOrOnTheLine(const glm::vec3& l1, const glm::vec3& l2, const glm::vec3& p);
	//float TriangleArea(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

	OpenList mQueue;
	std::deque<Triangle3D*> mTriangleWayPoint;
};