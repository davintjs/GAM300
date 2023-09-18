#include "Precompiled.h"

#include "Polygon.h"

Polygon3D::Polygon3D(const std::vector<glm::vec3>& positions)
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

Polygon3D::~Polygon3D()
{

}

glm::vec3 Polygon3D::GetNormal() const
{
	return mNormal;
}

Polygon3D::Orientation Polygon3D::GetOrientation() const
{
	return mOrientation;
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

// Need to take into consideration player radius here
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
		if (points[i].x < position.x || (points[i].x == position.x && points[i].y < position.y))
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
			std::sort(colinearPoints.begin(), colinearPoints.end(), [currentPosition](const glm::vec3& x, const glm::vec3& y) mutable
				{
					return glm::dot(x - currentPosition, x - currentPosition) < glm::dot(y - currentPosition, y - currentPosition);
				});

			for (int i = 0; i < colinearPoints.size(); ++i)
			{
				mPoints.push_back(colinearPoints[i]);
			}

			currentPosition = colinearPoints.back();

			// Remove all points that were added to the convex hull
			for (int i = 0; i < colinearPoints.size(); ++i)
			{
				int index = 0;
				for (int j = 0; j < tempPoint.size(); ++j)
				{
					if (tempPoint[j] == colinearPoints[i])
					{
						index = j;
						break;
					}
				}
				tempPoint.erase(tempPoint.begin() + index);
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

void Polygon3D::CalculateNormal(const std::vector<glm::vec3>& vertices)
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

void Polygon3D::JoinPolygon(Polygon3D& polygon)
{
	// TODO left

}

bool Polygon3D::HoleInPolygon(Polygon3D& mHole)
{
	for (int i = 0; i < mHole.GetPoints().size(); ++i)
	{
		int counter = 0;
		glm::vec2 pt = glm::vec2(mHole.GetPoints()[i]);
		glm::vec2 dir = glm::vec2(1000.f, 0.f); // Infinite horizontal line
		Segment2D line(pt, pt + dir);

		for (int j = 0; j < mPoints.size(); ++j)
		{
			glm::vec2 m1 = glm::vec2(mPoints[j]);
			glm::vec2 m2 = glm::vec2(mPoints[(j != mPoints.size() - 1) ? j + 1 : 0]);
			Segment2D polyLine(m1, m2);

			if ((polyLine.point1.y < pt.y) && (polyLine.point2.y < pt.y))
			{
				continue;
			}
			else if ((polyLine.point1.y >= pt.y) && (polyLine.point2.y >= pt.y))
			{
				continue;
			}
			else
			{
				float intersectionTime;
				if (Intersects(polyLine, line, &intersectionTime))
				{
					++counter;
				}
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
		Line3D line(pos, glm::vec3(0.f, 0.f, 1.f));

		float t;
		if (Intersects(line, plane, &t))
		{
			pos = line.lerp(t);
		}
	}

	mHole.CalculateNormal(mHole.GetPoints());

	return true;
}

void Polygon3D::SwitchOrientation()
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

float Polygon3D::PointLeftOfVecOrOnLine(const glm::vec3& l1, const glm::vec3& l2, const glm::vec3& p)
{
	return (l1.x - p.x) * (l2.y - p.y) - (l1.y - p.y) * (l2.x - p.x);
}

bool Polygon3D::Intersects(const Segment2D& seg1, const Segment2D& seg2, float* rt)
{
	glm::vec2 EP_line1 = seg1.point2 - seg1.point1;
	glm::vec2 EP_line2 = seg2.point2 - seg2.point1;
	float det = EP_line1.x * EP_line2.y - EP_line2.x * EP_line1.y;

	if (det == 0.f)
	{
		return false;
	}

	float t = (EP_line2.x * (seg1.point1.y - seg2.point1.y) + EP_line2.y * (seg2.point1.x - seg1.point1.x)) / det;
	float u = (EP_line1.x * (seg2.point1.y - seg1.point1.y) + EP_line1.y * (seg1.point1.x - seg2.point1.x)) / (EP_line2.x * EP_line1.y - EP_line2.y * EP_line1.x);

	if (rt != NULL)
	{
		*rt = t;
	}

	if (t >= 0.f && t <= 1.f)
	{
		if (u >= 0.f && u <= 1.f)
		{
			return true;
		}
	}

	return false;
}

bool Polygon3D::Intersects(const Line3D& line, const Plane3D& plane, float* rt)
{
	glm::vec3 P_Normal = plane.normal();

	float VdotN = glm::dot(line.vector, P_Normal);
	float MdotN = glm::dot(line.point, P_Normal);
	if (VdotN == 0.f)
	{
		return false;
	}

	float t = -((MdotN + plane[3]) / VdotN);
	if (rt != NULL)
	{
		*rt = t;
	}

	float Mtv = MdotN + (t * VdotN);
	float test = Mtv + plane[3];
	if (test != 0.f)
	{
		return false;
	}

	return true;

}