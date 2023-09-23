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

	Polygon3D(const std::vector<glm::vec3>& positions);
	~Polygon3D();

	// Getter functions
	glm::vec3 GetNormal() const;
	Orientation GetOrientation() const;
	std::vector<glm::vec3>& GetPoints();
	glm::vec3 GetMaxPoint();
	glm::vec3 GetMinPoint();

	bool HoleInPolygon(Polygon3D& mHole);
	void SwitchOrientation();
	void JoinPolygon(Polygon3D& polygon);
	void CalculateNormal(const std::vector<glm::vec3>& vertices);

private:
	void GenerateConvexHull(const std::vector<glm::vec3>& points);
	float PointLeftOfVecOrOnLine(const glm::vec3& l1, const glm::vec3& l2, const glm::vec3& p);
	bool Intersects(const Segment2D& seg1, const Segment2D& seg2, float* rt);
	bool Intersects(const Segment3D& seg1, const Segment3D& seg2, float* rt);
	bool Intersects(const Line3D& line, const Plane3D& plane, float* rt);
	bool Parallel(const glm::vec3& v1, const glm::vec3& v2);
	float CalculateSquaredDistance(const glm::vec3& mVec);

	glm::vec3 minPoint;
	glm::vec3 maxPoint;
	int mNumberOfPoints = 0;
	std::vector<glm::vec3> mPoints;
	Orientation mOrientation;
	glm::vec3 mNormal;
};

static bool isFrontFace(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
	float signedArea = (p2.x - p1.x) * (p3.y - p1.y) - (p3.x - p1.x) * (p2.y - p1.y);
	signedArea *= 0.5f;
	if (signedArea > 0.f)
	{
		return true;
	}
	return false;
}