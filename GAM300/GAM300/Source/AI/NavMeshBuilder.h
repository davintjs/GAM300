#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "Polygon.h"
#include "Triangle3D.h"
#include "Mesh.h"

class NavMeshBuilder
{
public:
	NavMeshBuilder(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices);
	~NavMeshBuilder();

	Mesh* CreateMesh();

	std::vector<Polygon>& GetRegion();
	std::vector<Polygon>& GetHoles();

private:
	std::vector<Polygon> ComputeRegions(const std::vector<Triangle3D>& GroundTriangles);
	std::vector<Triangle3D> GetGroundTriangles(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices);
	void SetBoundary(Polygon* mBoundary);
	void AddHole(Polygon* hole);
	void RemoveHoles();
	std::vector<Triangle3D> Triangulate();

	int mTriCount = 0;
	Polygon* mBoundary;
	std::vector<Polygon> mHoles;
	std::vector<Polygon> mObstacles;
	std::vector<Polygon> mRegion;
};