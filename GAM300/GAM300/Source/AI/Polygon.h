/*!***************************************************************************************
\file			Polygon.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the declarations of the following:
	1. Polygon3D class
		a. A polygon of a custom shape for navmesh generation (Can also be a hole)
		b. Addition of holes into the polygon
		c. Joining of polygon with holes
		d. Generation of convex hull points for triangulation
		e. Helper functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include <vector>

#include "glm/glm.hpp"
#include "Geometry.h"

class Polygon3D
{
public:
	enum class Orientation
	{
		CLOCKWISE,
		COUNTERCLOCKWISE
	};

	Polygon3D(const std::vector<glm::vec3>& positions, const int& mRegionID);
	~Polygon3D();

	// Getter functions
	// Returns the normal of the polygon
	glm::vec3 GetNormal() const;

	const glm::vec3& GetBarycenter() const;

	// Returns the orientation of the polygon
	const Orientation& GetOrientation() const;

	// Returns the points of this polygon
	std::vector<glm::vec3>& GetPoints();

	// Returns the max point of this polygon
	glm::vec3 GetMaxPoint();

	// Returns the min point of this polygon
	glm::vec3 GetMinPoint();

	// Checks if the hole is in this polygon
	bool HoleInPolygon(Polygon3D& mHole);

	// Switch the orientation of this polygon to clockwise or counter-clockwise
	void SwitchOrientation();

	// Join this polygon with the argument polygon (Holes)
	void JoinPolygon(Polygon3D& polygon);

	// Calculate the normal of the given vertices
	void CalculateNormalBarycenter(const std::vector<glm::vec3>& vertices);

private:
	// Generate the convex hull given the ground vertices
	void GenerateConvexHull(const std::vector<glm::vec3>& points);

	void GeneratePointsCCW(const std::vector<glm::vec3>& points);

	std::vector<glm::vec3> TrimPositions(const std::vector<glm::vec3>& positions);

	// Helper functions
	// Determines if the point is on the left or on the vector
	float PointLeftOfVecOrOnLine(const glm::vec3& l1, const glm::vec3& l2, const glm::vec3& p);

	// Calculation of squared distance of the vector
	float CalculateSquaredDistance(const glm::vec3& mVec);

	bool mPendingRebake = false;
	int mRegionID = 0;
	glm::vec3 minPoint;
	glm::vec3 maxPoint;
	glm::vec3 mBarycenterPoint;
	int mNumberOfPoints = 0;
	std::vector<glm::vec3> mPoints;
	Orientation mOrientation;
	glm::vec3 mNormal;
};

// Returns if the three points give us a front face triangle
inline float isFrontFace(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	float signedArea = (p2.x - p1.x) * (p3.z - p1.z) - (p3.x - p1.x) * (p2.z - p1.z);
	return signedArea;
}