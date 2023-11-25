/*!***************************************************************************************
\file			Polygon.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
	This file contains the definitions of the following:
	1. Polygon3D class
		a. A polygon of a custom shape for navmesh generation (Can also be a hole)
		b. Addition of holes into the polygon
		c. Joining of polygon with holes
		d. Generation of convex hull points for triangulation
		e. Helper functions

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "Polygon.h"

Polygon3D::Polygon3D(const std::vector<glm::vec3>& positions, const int& regionID)
{
	mOrientation = Orientation::COUNTERCLOCKWISE;
	//GeneratePointsCCW(positions);
	std::vector<glm::vec3> mTrimmedPos = TrimPositions(positions); // Filter positions to edges only
	GenerateConvexHull(positions);
	CalculateNormalBarycenter(mPoints);

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
	}

	this->minPoint = minPoint;
	this->maxPoint = maxPoint;

	mNumberOfPoints = static_cast<int>(mPoints.size());
	mRegionID = regionID;
}

Polygon3D::~Polygon3D()
{
	
}

glm::vec3 Polygon3D::GetNormal() const
{
	return mNormal;
}

const Polygon3D::Orientation& Polygon3D::GetOrientation() const
{
	return mOrientation;
}

const glm::vec3& Polygon3D::GetBarycenter() const
{
	return mBarycenterPoint;
}

std::vector<glm::vec3>& Polygon3D::GetPoints()
{
	return mPoints;
}

glm::vec3 Polygon3D::GetMaxPoint()
{
	return maxPoint;
}

glm::vec3 Polygon3D::GetMinPoint()
{
	return minPoint;
}

void Polygon3D::GeneratePointsCCW(const std::vector<glm::vec3>& points)
{
	// Get the barycenter point of the points
	glm::vec3 _BaryCenterPoint = { 0.f, 0.f, 0.f };
	for (const auto& p : points)
	{
		_BaryCenterPoint += p;
	}
	_BaryCenterPoint /= static_cast<int>(points.size());

	// Split the points on upper semi-circle and lower semi-circle based on their z-value comparing to _BaryCenterPoint
	std::vector<glm::vec3> mLowerSemiCircle;
	std::vector<glm::vec3> mUpperSemiCircle;
	for (const auto& _point : points)
	{
		if (_point.z <= _BaryCenterPoint.z)
		{
			mLowerSemiCircle.push_back(_point);
		}
		else
		{
			mUpperSemiCircle.push_back(_point);
		}
	}

	// Reaching here we have partitioned the points in upper and lower semi-circle

	// For lower semi-circle, we sort based on least x-value first, if it is tied we sort based on least z-value
	std::sort(mLowerSemiCircle.begin(), mLowerSemiCircle.end(), [&](const glm::vec3& lhs, const glm::vec3& rhs)
		{
			if (lhs.x == rhs.x)
			{
				return lhs.z < rhs.z;
			}
			return lhs.x < rhs.x;
		});

	// For upper semi-circle, we sort based on x-value most first, if it is tied we sort based on least z-value
	std::sort(mUpperSemiCircle.begin(), mUpperSemiCircle.end(), [&](const glm::vec3& lhs, const glm::vec3& rhs)
		{
			if (lhs.x == rhs.x)
			{
				return lhs.z < rhs.z;
			}
			return lhs.x > rhs.x;
		});

	mPoints = mLowerSemiCircle;
	for (int i = 0; i < mUpperSemiCircle.size(); ++i)
	{
		mPoints.push_back(mUpperSemiCircle[i]);
	}

}

void Polygon3D::GenerateConvexHull(const std::vector<glm::vec3>& points) 
{
	// If already only 3 points given, return them based on front faced or back faced
	if (points.size() == 3)
	{
		if (isFrontFace(points[0], points[1], points[2]))
		{
			mPoints = points;
			mOrientation = Orientation::COUNTERCLOCKWISE;
		}
		else
		{
			mPoints.push_back(points[2]);
			mPoints.push_back(points[1]);
			mPoints.push_back(points[0]);
			mOrientation = Orientation::CLOCKWISE;
		}
		return;
	}

	if (points.size() < 3)
	{
		return;
	}

	// Step 1: Find vertex with the smallest x coordinate
	// If there are several of them, find the one with the smallest y coordinate
	glm::vec3 position = points[0];

	int index = 0;
	for (int i = 1; i < points.size(); ++i)
	{
		if (points[i].x < position.x || (points[i].x == position.x && points[i].z < position.z))
		{
			position = points[i];
			index = i;
		}
	}

	mPoints.push_back(position);

	std::vector<glm::vec3> tempPoint(points);
	tempPoint.erase(tempPoint.begin() + index); // Remove the point

	// Step 2: Loop through to generate the convex hull
	glm::vec3 currentPosition = mPoints[0];

	// Store all colinear points
	std::vector<glm::vec3> colinearPoints;

	int counter = 0;

	while (true)
	{
		// Add the start position again after 2 iterations
		if (counter == 2)
		{
			tempPoint.push_back(mPoints[0]);
		}

		glm::vec3 nextPosition;
		if (tempPoint.size() == 1)
		{
			nextPosition = tempPoint[0];
		}
		else if (tempPoint.size() == 0) // Means we are done generating the convex hull
		{
			break;
		}
		else
		{
			nextPosition = tempPoint[rand() % (tempPoint.size() - 1)];
		}

		// Test if there is a point to the right of currPosition and nextPosition
		for (int i = 0; i < tempPoint.size(); ++i)
		{
			// Skip if is equal to nextPosition
			if (tempPoint[i] == nextPosition)
			{
				continue;
			}

			float relation = PointLeftOfVecOrOnLine(currentPosition, nextPosition, tempPoint[i]);

			if (relation < 0.00001f && relation > -0.00001f)
			{
				colinearPoints.push_back(tempPoint[i]);
			}
			else if (relation < 0.f)
			{
				nextPosition = tempPoint[i];
				colinearPoints.clear();
			}
		}

		// If there are colinear points
		if (colinearPoints.size() > 0)
		{
			colinearPoints.push_back(nextPosition);

			// Sort the container in ascending order based on x coordinate
			std::sort(colinearPoints.begin(), colinearPoints.end(), [currentPosition](const glm::vec3& x, const glm::vec3& z) mutable
				{
					return glm::dot(x - currentPosition, x - currentPosition) < glm::dot(z - currentPosition, z - currentPosition);
				});

			for (int i = 0; i < colinearPoints.size(); ++i)
			{
				mPoints.push_back(colinearPoints[i]);
			}

			currentPosition = colinearPoints.back();

			// Remove all points that were added to the convex hull
			for (int i = 0; i < colinearPoints.size(); ++i)
			{
				int index2 = 0;
				for (int j = 0; j < tempPoint.size(); ++j)
				{
					if (tempPoint[j] == colinearPoints[i])
					{
						index2 = j;
						break;
					}
				}
				tempPoint.erase(tempPoint.begin() + index2);
			}

			colinearPoints.clear();
		}
		else // No colinear points
		{
			mPoints.push_back(nextPosition);
			for (int i = 0; i < tempPoint.size(); ++i)
			{
				if (tempPoint[i] == nextPosition)
				{
					tempPoint.erase(tempPoint.begin() + i);
					break;
				}
			}
			currentPosition = nextPosition;
		}

		if (currentPosition == mPoints[0])
		{
			mPoints.pop_back();
			break;
		}
		++counter;
	}
}

std::vector<glm::vec3> Polygon3D::TrimPositions(const std::vector<glm::vec3>& positions)
{
	std::vector<glm::vec3> mResVec = positions; // Our returning vector

	// Filter the convex hull to only give us the edges of the polygon

	// Sort according to same z value but different x value
	std::sort(mResVec.begin(), mResVec.end(), [](glm::vec3& p1, glm::vec3& p2)
		{
			return std::tie(p1.z, p1.x) < std::tie(p2.z, p2.x);
		});

	// Find the smallest x value and largest x value with the same z value
	float mCurrZValue = mResVec[0].z;
	bool done = false;

	for (int i = 1; i < mResVec.size(); ++i)
	{
		while (mResVec[i].z == mCurrZValue)
		{
			if ((i + 1) >= mResVec.size())
			{
				done = true;
				break;
			}
			else if (mResVec[i + 1].z != mCurrZValue) // Reached the end for this z value
			{
				break;
			}

			mResVec.erase(mResVec.begin() + i);
		}

		if (!done)
		{
			++i;
			mCurrZValue = mResVec[i].z;
		}
		else
		{
			break;
		}
	}

	// Sort according to same x value but different z value
	std::sort(mResVec.begin(), mResVec.end(), [](glm::vec3& p1, glm::vec3& p2)
		{
			return p1.x < p2.x; // Descending order
		});

	// Find smallest z value and largest z value with the same x value
	float mCurrXValue = mResVec[0].x;
	bool mDone = false;

	for (int i = 1; i < mResVec.size(); ++i)
	{
		while (mResVec[i].x == mCurrXValue)
		{
			if ((i + 1) >= mResVec.size())
			{
				mDone = true;
				break;
			}
			else if (mResVec[i + 1].x != mCurrXValue) // Reached the end for this x value
			{
				break;
			}

			mResVec.erase(mResVec.begin() + i);
		}

		if (!mDone)
		{
			++i;
			mCurrXValue = mResVec[i].x;
		}
		else
		{
			break;
		}
	}

	return mResVec;
}

void Polygon3D::CalculateNormalBarycenter(const std::vector<glm::vec3>& vertices)
{
	glm::vec3 normal{0.f, 0.f, 0.f};
	glm::vec3 barycenter{ 0.f, 0.f, 0.f };

	for (int i = 0; i < vertices.size(); i++)
	{
		int j = (i + 1) % vertices.size();
		normal.x += (vertices[i].y - vertices[j].y) * (vertices[i].z + vertices[j].z);
		normal.y += (vertices[i].z - vertices[j].z) * (vertices[i].x + vertices[j].x);
		normal.z += (vertices[i].x - vertices[j].x) * (vertices[i].y + vertices[j].y);

		barycenter += vertices[i];
	}

	mNormal = glm::normalize(normal);
	mBarycenterPoint = barycenter / static_cast<float>(vertices.size());
}

// Joins this polygon with the hole polygon (Argument)
void Polygon3D::JoinPolygon(Polygon3D& polygon)
{
	glm::vec3 rightVertex;
	rightVertex.x = -FLT_MAX;
	rightVertex.y = -FLT_MAX;
	int rightVertexPos = -1;

	// Find the right most vertex of this hole
	for (int i = 0; i < polygon.GetPoints().size(); ++i)
	{
		if (polygon.GetPoints()[i].x > rightVertex.x)
		{
			rightVertex = polygon.GetPoints()[i]; // Set the right vertex to this vertex
			rightVertexPos = i; // Store the position of this right vertex
		}
	}

	glm::vec3 tempSecondPoint = rightVertex + (100000.f * glm::vec3(1.f, 0.f, 0.f));
	
	Segment3D infiniteLine(rightVertex, tempSecondPoint); // Starting from the right most vertex of the hole's polygon, extend the line infinitely to the right

	// Find the closest edge of the boundary polygon that intersects the infinite line
	glm::vec3 intersectionPoint;
	glm::vec3 rightPointOfLine;

	float closestDistance = FLT_MAX;
	int candidatePos = -1;
	for (int i = 0; i < GetPoints().size(); ++i)
	{
		const std::vector<glm::vec3>& cornerVec = GetPoints();

		glm::vec3 firstPoint = { cornerVec[i].x, cornerVec[i].y, cornerVec[i].z };

		int j = i + 1 == cornerVec.size() ? 0 : i + 1;
		glm::vec3 secondPoint = { cornerVec[j].x, cornerVec[j].y, cornerVec[j].z };

		Segment3D currLine(firstPoint, secondPoint); // Current line we are checking of this polygon

		float intersectionTime;
		if (Intersects(currLine, infiniteLine, &intersectionTime))
		{
			glm::vec3 tempIntersectionPoint = currLine.lerp(intersectionTime);
			glm::vec3 distanceVec = { tempIntersectionPoint.x - rightVertex.x, tempIntersectionPoint.y - rightVertex.y, tempIntersectionPoint.z - rightVertex.z };

			// Calculate the distance between this intersection point and the rightVertex
			float distance = glm::length(distanceVec);
			if (distance < closestDistance)
			{
				intersectionPoint = tempIntersectionPoint; // Store the intersection point
				closestDistance = distance; // Store the closest distance to this
				rightPointOfLine = secondPoint; // Store the right most point of the line which contains the intersection point
				candidatePos = i;
			}
		}
	}

	// Reaching here means we have found the intersection point between the infinite line and the polygon edge and the right most point of that polygon edge

	// Create the triangle based on the right vertex of the hole, the intersection point, and the right point of the line segment of the polygon
	Triangle3D mTri(glm::vec3(rightVertex.x, rightVertex.y, rightVertex.z), intersectionPoint, rightPointOfLine);

	float theta = FLT_MAX;

	// Pre-compute the length of the a side of the triangle as this has no change
	glm::vec3 aSide = { intersectionPoint.x - rightVertex.x, intersectionPoint.y - rightVertex.y, intersectionPoint.z - rightVertex.z };
	float a = CalculateSquaredDistance(aSide);

	glm::vec3 candidate{}; // This will store the point that has the lowest theta value in the triangle formed
	bool hasCandidateInTri = false;

	// Check if the triangle contains any of the vertices of the polygon
	for (int j = 0; j < GetPoints().size(); ++j)
	{
		const std::vector<glm::vec3> mCornerVecs = GetPoints();
		glm::vec3 currVert = { mCornerVecs[j].x, mCornerVecs[j].y, mCornerVecs[j].z };

		if (mTri.ContainsPoint(currVert))
		{
			// Calculate the angle theta using cosine law
			glm::vec3 cSide = { currVert.x - intersectionPoint.x, currVert.y - intersectionPoint.y, currVert.z - intersectionPoint.z };
			float c = CalculateSquaredDistance(cSide);

			glm::vec3 bSide = { currVert.x - rightVertex.x, currVert.y - rightVertex.y, currVert.z - rightVertex.z };
			float b = CalculateSquaredDistance(bSide);

			float tempTheta = acos((c - a - b) / (-2 * a * b));
			if (tempTheta < theta) // Found a smaller theta than any previously compared theta
			{
				hasCandidateInTri = true;
				theta = tempTheta; // Set theta tracker to this tempTheta
				candidate = currVert; // Store the currVert as our candidate
				candidatePos = j + 1; // Store the candidate position
			}
		}
	}

	if (!hasCandidateInTri)
	{
		candidate = rightPointOfLine; // Means there was no vertices of the polygon that lie inside the triangle
	}

	// Reaching here means we have found the candidate that minimizes the angle theta

	// Bridge the polygon and the hole between the rightVertex and the candidate by creating
	// an invisible line segment between the candidate and the rightVertex
	glm::vec3 dupRightVertex = rightVertex;
	glm::vec3 dupCandidate = candidate;

	int tracker = 0;
	std::vector<glm::vec3>& p = GetPoints();
	p.insert(GetPoints().begin() + candidatePos, rightVertex);

	for (int k = rightVertexPos + 1; k < polygon.GetPoints().size(); ++k)
	{
		p.insert(GetPoints().begin() + candidatePos + (++tracker),
			polygon.GetPoints()[k]);
	}

	for (int s = 0; s < rightVertexPos; ++s) // Fill in the vertices from the start up till rightVertex
	{
		p.insert(GetPoints().begin() + candidatePos + (++tracker),
			polygon.GetPoints()[s]);
	}

	// Reaching here means we have done putting in the hole's vertices into the boundary

	// Insert the invisible line segment to connect the hole and polygon
	p.insert(GetPoints().begin() + candidatePos + (++tracker), dupRightVertex);
	p.insert(GetPoints().begin() + candidatePos + (++tracker), dupCandidate);
}

bool Polygon3D::HoleInPolygon(Polygon3D& mHole)
{
	for (int i = 0; i < mHole.GetPoints().size(); ++i)
	{
		int counter = 0;
		glm::vec2 pt = glm::vec2(mHole.GetPoints()[i].x, mHole.GetPoints()[i].z);
		glm::vec2 dir = glm::vec2(1000.f, 0.f); // Infinite horizontal line
		Segment2D line(pt, pt + dir);

		for (int j = 0; j < mPoints.size(); ++j)
		{
			glm::vec2 m1 = glm::vec2(mPoints[j].x, mPoints[j].z);
			glm::vec2 m2 = glm::vec2(mPoints[(j != mPoints.size() - 1) ? j + 1 : 0].x, mPoints[(j != mPoints.size() - 1) ? j + 1 : 0].z);
			Segment2D polyLine(m1, m2);

			float intersectionTime;
			if (Intersects(polyLine, line, &intersectionTime))
			{
				++counter;
			}
		}

		if (counter % 2 == 0)
		{
			return false;
		}
	}

	// Update hole according to current polygon's plane
	glm::vec3 n = this->GetNormal();

	float d = -glm::dot(n, (this->GetMaxPoint() + this->GetMinPoint()) / 2.f);
	Plane3D plane(n[0], n[1], n[2], d);

	for (int i = 0; i < mHole.GetPoints().size(); ++i)
	{
		glm::vec3& pos = mHole.GetPoints()[i];
		Line3D line(pos, glm::vec3(0.f, 1.f, 0.f));

		float t;
		if (Intersects(line, plane, &t))
		{
			pos = line.lerp(t);
		}
	}

	mHole.CalculateNormalBarycenter(mHole.GetPoints());

	return true;
}

void Polygon3D::SwitchOrientation()
{
	mOrientation = (mOrientation == Orientation::COUNTERCLOCKWISE) ? Orientation::CLOCKWISE : Orientation::COUNTERCLOCKWISE; // Change the orientation

	int leftTracker = 1; // Start from 2nd index as 1st index no change
	int rightTracker = static_cast<int>(mPoints.size()) - 1; // Start from last index

	while (leftTracker < rightTracker)
	{
		glm::vec3 tempHolder = mPoints[leftTracker]; // Hold the left one first
		mPoints[leftTracker] = mPoints[rightTracker]; // Set the left one to the right one
		mPoints[rightTracker] = tempHolder; // Set the right one to be the initial left one

		++leftTracker;
		--rightTracker;
	}
}

float Polygon3D::PointLeftOfVecOrOnLine(const glm::vec3& l1, const glm::vec3& l2, const glm::vec3& p)
{
	return (l1.x - p.x) * (l2.z - p.z) - (l1.z - p.z) * (l2.x - p.x);
}

float Polygon3D::CalculateSquaredDistance(const glm::vec3& mVec)
{
	return ((mVec.x * mVec.x) + (mVec.y * mVec.y) + (mVec.z * mVec.z));
}