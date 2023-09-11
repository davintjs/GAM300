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

	// Getter functions
	glm::vec3 GetNormal() const;
	Orientation GetOrientation() const;
	std::vector<glm::vec3>& GetPoints();
	glm::vec3 GetMaxPoint();
	glm::vec3 GetMinPoint();

	bool HoleInPolygon(Polygon& mHole);
	void SwitchOrientation();
	void JoinPolygon(Polygon& polygon);
	void CalculateNormal(const std::vector<glm::vec3>& vertices);

private:
	void GenerateConvexHull(const std::vector<glm::vec3>& points);

	glm::vec3 minPoint;
	glm::vec3 maxPoint;
	int mNumberOfPoints = 0;
	std::vector<glm::vec3> mPoints;
	Orientation mOrientation;
	glm::vec3 mNormal;
};