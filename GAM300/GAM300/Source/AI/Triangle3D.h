#pragma once

#include <vector>
#include "glm/glm.hpp"

enum class onList // For pathfinding
{
	NONE,
	OPEN_LIST,
	CLOSED_LIST
};

class Triangle3D
{
public:
	Triangle3D(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	~Triangle3D();

	std::vector<Triangle3D*> GetNeighbours();
	const glm::vec3 GetMidPoint() const;
	const glm::vec3 GetNormal() const;

	bool ContainsPoint(const glm::vec3& mPoint) const;

private:
	void AddNeighbour(Triangle3D* mTri);

	// NavMesh stuff
	Triangle3D* mParent;
	glm::vec3 mMidPoint;
	glm::vec3 mNormal;
	std::vector<Triangle3D*> mNeighbours;

	// Pathfinding stuff
	float mFinalCost = 0.f;
	float mHeuCost = 0.f;
	float mGivenCost = 0.f;
	onList mOnList = onList::NONE;
};