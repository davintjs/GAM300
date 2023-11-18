#pragma once

#include <queue>

class Triangle3D;

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
	bool operator<(const PathNode& rhs);

protected:
	Triangle3D* mParent;
	float mFinalCost = 0.f;
	float mHeuCost = 0.f;
	float mGivenCost = 0.f;
	OnList mOnList = OnList::NONE;
};

class AStarPather
{
public:
	bool ComputePath(const Triangle3D*& mStart, const Triangle3D*& mEnd);
	std::vector<glm::vec3> PathPostProcess(const glm::vec3& mStart, const glm::vec3& mEnd);

private:
	float CalculateHeuristic(const Triangle3D* mCurrNode, const Triangle3D*& mEnd);

	// Post-processing
	std::vector<std::pair<glm::vec3, glm::vec3>> GetPortals(const glm::vec3& mStart, const glm::vec3& mEnd);
	std::vector<glm::vec3> Funnel(const glm::vec3& mStart, const glm::vec3& mEnd, const std::vector<std::pair<glm::vec3, glm::vec3>>& mPortals);

	float IsAPointLeftOfVectorOrOnTheLine(const glm::vec3& l1, const glm::vec3& l2, const glm::vec3& p);
	float TriangleArea(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);

	std::priority_queue<Triangle3D*> mQueue;
	std::vector<Triangle3D*> mTriangleWayPoint;
	std::vector<PathNode*> mVisitedNodes;

};