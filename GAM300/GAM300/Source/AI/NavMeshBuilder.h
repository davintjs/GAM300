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

	std::vector<Polygon>& GetRegion();
	std::vector<Polygon>& GetHoles();

private:
	std::vector<Polygon> ComputeRegions(const std::vector<Triangle3D>& GroundTriangles);
	std::vector<Triangle3D> GetGroundTriangles(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices);
	void SetBoundary(Polygon* mBoundary);
	void AddHole(Polygon* hole);
	void RemoveHoles();
	std::vector<Triangle3D> Triangulate();
	void InitializeGroundTriangles(std::vector<Triangle3D>& GroundTriangles);
	void FindAllNeighboursWithNormal(std::vector<Triangle3D>& triPos, Triangle3D neighbourTri, const glm::vec3 triNormal);

	// Helper functions
	template <typename T>
	int sign(const T& val);
	int IsTriangleOrientedClockWise(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n);
	bool PointsOnLine(std::vector<glm::vec3> points, const glm::vec3& n);
	bool isFrontFace(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
	bool Parallel(const glm::vec3& v1, const glm::vec3& v2);

	int mTriCount = 0;
	Polygon* mBoundary;
	std::vector<Polygon> mHoles;
	std::vector<Polygon> mObstacles;
	std::vector<Polygon> mRegion;
};