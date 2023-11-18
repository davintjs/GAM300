/*!***************************************************************************************
\file			NavMeshBuilder.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. NavMeshBuilder singleton class
		a. Builds the navmesh given the ground vertices and indices
		b. Add holes and obstacles to the navmesh
		c. Triangulation by ear clipping method

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include <vector>
#include "glm/glm.hpp"

#include "Polygon.h"
#include "Core/Events.h"

#define NAVMESHBUILDER NavMeshBuilder::Instance()

class NavMesh;

SINGLETON(NavMeshBuilder)
{
public:
	NavMeshBuilder() {};
	~NavMeshBuilder() {};

	void Init();

	// Get all grounds of the current scene
	std::pair<std::vector<glm::vec3>, std::vector<glm::ivec3>> GetAllGrounds();

	// Get all obstacles of the current scene
	void GetAllObstacles();

	// NavMeshBuilder exit
	void Exit();

	// Builds the navmesh from the given vertices and indices
	void BuildNavMesh();
	NavMesh* CreateNavMesh();

	// Rebake the navmesh
	void Rebake();

	// Getter functions
	// Returns the region of the navmesh
	std::vector<Polygon3D>& GetRegion();

	// Returns the holes in this navmesh
	//std::vector<Polygon3D>& GetHoles();

	// Add obstacle to this navmesh
	void AddObstacle(Polygon3D* mObstacle);

	// Returns the obstacles in this navmesh
	std::vector<Polygon3D>& GetObstacles();

	NavMesh* GetNavMesh() { return mNavMesh; }

private:
	// Compute the individual regions of the navmesh
	std::vector<Polygon3D> ComputeRegions(const std::vector<Triangle3D>& GroundTriangles);

	// Returns the ground triangles of the navmesh
	std::vector<Triangle3D> GetGroundTriangles(const std::vector<glm::vec3>& GroundVertices, const std::vector<glm::ivec3>& GroundIndices);

	void OffsetRadius(const float& mRadius);
	void ObstacleOffset(const float& mRadius);

	// Add obstacle to the navmesh and editing the navmesh to accomodate the holes
	void RemoveObstaclesFromMesh();

	// Triangulation of the navmesh with ear clipping method
	std::vector<Triangle3D> Triangulate();

	// Initialize ground triangles after triangulation
	void InitializeGroundTriangles(std::vector<Triangle3D>& GroundTriangles);

	// Find neighbours with the same normal (Helper function for region creation)
	void FindAllNeighboursWithNormal(std::vector<Triangle3D>& triPos, Triangle3D neighbourTri, const glm::vec3 triNormal);

	// Helper functions
	// Returns of the value is positive or negative
	template <typename T>
	int sign(const T& val);

	// Determines if triangle is clockwise oriented
	int IsTriangleOrientedClockWise(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, const glm::vec3& n);

	// Checks if the constructed triangle is a clockwise oriented triangle
	bool PointsOnLine(std::vector<glm::vec3> points, const glm::vec3& n);

	// Parallel check of the two vectors
	bool Parallel(const glm::vec3& v1, const glm::vec3& v2);

	void CallbackContactAdd(ContactAddedEvent* pEvent);
	void CallbackContactRemove(ContactRemovedEvent* pEvent);

	int mObstacleCount = 0;
	int mRegionCount = 0;
	int mTriCount = 0;
	int mIndexCount = 0;
	Polygon3D* mBoundary;
	NavMesh* mNavMesh;
	//std::vector<Polygon3D> mHoles;
	std::vector<Polygon3D> mObstacles;
	std::vector<Polygon3D> mRegions; // Contains the total regions of the game
};