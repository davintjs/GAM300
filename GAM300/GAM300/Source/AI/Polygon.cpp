#include "Precompiled.h"

#include "Polygon.h"

Polygon::Polygon(const std::vector<glm::vec3>& positions)
{
	mOrientation = Orientation::COUNTERCLOCKWISE;
	GenerateConvexHull(positions);
	CalculateNormal(mPoints);

	minPoint = { FLT_MAX, FLT_MAX, FLT_MAX };
	maxPoint = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
	for (int i = 0; i < mPoints.size(); i++)
	{
		minPoint.x = std::min(minPoint.x, mPoints[i].x);
		minPoint.y = std::min(minPoint.y, mPoints[i].y);
		minPoint.z = std::min(minPoint.z, mPoints[i].z);

		maxPoint.x = std::max(maxPoint.x, mPoints[i].x);
		maxPoint.y = std::max(maxPoint.y, mPoints[i].y);
		maxPoint.z = std::max(maxPoint.z, mPoints[i].z);

		glm::vec3 p = mPoints[i];
	}

	mNumberOfPoints = mPoints.size();
}

Polygon::~Polygon()
{

}

glm::vec3 Polygon::GetNormal() const
{
	return mNormal;
}

Polygon::Orientation Polygon::GetOrientation() const
{
	return mOrientation;
}

std::vector<glm::vec3>& Polygon::GetPoints()
{
	return mPoints;
}

glm::vec3 Polygon::GetMaxPoint()
{
	return maxPoint;
}


glm::vec3 Polygon::GetMinPoint()
{
	return minPoint;
}

void Polygon::GenerateConvexHull(const std::vector<glm::vec3>& points)
{

}

void Polygon::CalculateNormal(const std::vector<glm::vec3>& vertices)
{
	glm::vec3 normal{0.f, 0.f, 0.f};

	for (int i = 0; i < vertices.size(); i++)
	{
		int j = (i + 1) % vertices.size();
		normal.x += (vertices[i].y - vertices[j].y) * (vertices[i].z + vertices[j].z);
		normal.y += (vertices[i].z - vertices[j].z) * (vertices[i].x + vertices[j].x);
		normal.z += (vertices[i].x - vertices[j].x) * (vertices[i].y + vertices[j].y);
	}

	mNormal = glm::normalize(normal);
}

void Polygon::JoinPolygon(Polygon& polygon)
{

}

void Polygon::HoleInPolygon(Polygon& mHole)
{

}

void Polygon::SwitchOrientation()
{
	mOrientation == Orientation::CLOCKWISE ? Orientation::COUNTERCLOCKWISE : Orientation::CLOCKWISE; // Change the orientation

	int leftTracker = 1; // Start from 2nd index as 1st index no change
	int rightTracker = mPoints.size() - 1; // Start from last index

	while (leftTracker < rightTracker)
	{
		glm::vec3 tempHolder = mPoints[leftTracker]; // Hold the left one first
		mPoints[leftTracker] = mPoints[rightTracker]; // Set the left one to the right one
		mPoints[rightTracker] = tempHolder; // Set the right one to be the initial left one

		++leftTracker;
		--rightTracker;
	}
}