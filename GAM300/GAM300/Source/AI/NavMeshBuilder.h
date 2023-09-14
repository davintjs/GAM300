#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "Polygon.h"
#include "Triangle3D.h"
#include "NavMesh.h"

class NavMeshBuilder
{
public:
	NavMeshBuilder(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices);
	~NavMeshBuilder();

	NavMesh* CreateNavMesh();

	std::vector<Polygon3D>& GetRegion();
	std::vector<Polygon3D>& GetHoles();

private:
	std::vector<Polygon3D> ComputeRegions(const std::vector<Triangle3D>& GroundTriangles);
	std::vector<Triangle3D> GetGroundTriangles(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices);
	void SetBoundary(Polygon3D* mBoundary);
	void AddHole(Polygon3D* hole);
	void RemoveHoles();
	std::vector<Triangle3D> Triangulate();
	void InitializeGroundTriangles(std::vector<Triangle3D>& GroundTriangles);
	void FindAllNeighboursWithNormal(std::vector<Triangle3D>& triPos, Triangle3D neighbourTri, const glm::vec3 triNormal);

	// Helper functions
	template <typename T>
	int sign(const T& val);
	int IsTriangleOrientedClockWise(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n);
	bool PointsOnLine(std::vector<glm::vec3> points, const glm::vec3& n);
	bool Parallel(const glm::vec3& v1, const glm::vec3& v2);

	int mTriCount = 0;
	Polygon3D* mBoundary;
	std::vector<Polygon3D> mHoles;
	std::vector<Polygon3D> mObstacles;
	std::vector<Polygon3D> mRegion;
};