#pragma once

#include <vector>
#include "glm/glm.hpp"

class Polygon
{
public:
	enum class Orientation
	{
		CLOCKWISE,
		COUNTERCLOCKWISE
	};

	Polygon(const std::vector<glm::vec3>& positions);
	~Polygon();

	glm::vec3 GetNormal() const;
	Orientation GetOrientation() const;
	std::vector<glm::vec3>& GetPoints();
	glm::vec3 GetMaxPoint();
	glm::vec3 GetMinPoint();

private:
	void GenerateConvexHull(const std::vector<glm::vec3>& points);
	void CalculateNormal(const std::vector<glm::vec3>& vertices);
	void JoinPolygon(Polygon& polygon);
	void HoleInPolygon(Polygon& mHole);
	void SwitchOrientation();

	glm::vec3 minPoint;
	glm::vec3 maxPoint;
	int mNumberOfPoints = 0;
	std::vector<glm::vec3> mPoints;
	Orientation mOrientation;
	glm::vec3 mNormal;
};