#pragma once

#include <vector>
#include "Triangle3D.h"

class NavMesh
{
public:
	NavMesh(std::vector<Triangle3D> mTri);
	~NavMesh();

	const std::vector<Triangle3D> GetNavMeshTriangles() const;
	const std::vector<glm::vec3> GetPoints() const;
	const std::vector<unsigned int> GetIndices() const;

	// Pathfinding stuff
	bool FindPath(glm::vec3& mStart, glm::vec3& mEnd);

private:
	void LinkTriangles(Triangle3D* mTri1, Triangle3D* mTri2);
	void LinkAllTriangles();
	void UpdateNavMesh();

	std::vector<Triangle3D> mTriangles;
	std::vector<glm::vec3> mPoints;
	std::vector<unsigned int> mIndices;
};