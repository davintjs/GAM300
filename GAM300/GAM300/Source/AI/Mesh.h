#pragma once

#include <vector>
#include "Triangle3D.h"

class Mesh
{
public:
	Mesh(std::vector<Triangle3D> mTri);
	~Mesh();

	const std::vector<Triangle3D> GetMeshTriangles() const;
	const std::vector<glm::vec3> GetPoints() const;
	const std::vector<unsigned int> GetIndices() const;

	// Pathfinding stuff
	bool FindPath(glm::vec3& mStart, glm::vec3& mEnd);

private:
	void LinkTriangles(Triangle3D* mTri1, Triangle3D* mTri2);
	void LinkAllTriangles();
	void UpdateMesh();

	std::vector<Triangle3D> mTriangles;
	std::vector<glm::vec3> mPoints;
	std::vector<unsigned int> mIndices;
};