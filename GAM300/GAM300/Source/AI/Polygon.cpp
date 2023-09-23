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

	glm::vec3 secondPoint = rightVertex + (100000.f * glm::vec3(1.f, 0.f, 0.f)); // Might need to do rounding for floating error??
	
	Segment3D infiniteLine(rightVertex, secondPoint); // Starting from the right most vertex of the hole's polygon, extend the line infinitely to the right

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

	glm::vec3 candidate; // This will store the point that has the lowest theta value in the triangle formed
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

bool Polygon3D::Intersects(const Segment3D& seg1, const Segment3D& seg2, float* rt)
{
	float A = seg1.point2.x - seg1.point1.x;
	float B = seg2.point1.x - seg2.point2.x;
	float C = seg2.point1.x - seg1.point1.x;

	float D = seg1.point2.y - seg1.point1.y;
	float E = seg2.point1.y - seg2.point2.y;
	float F = seg2.point1.y - seg1.point1.y;

	float t = (C * E - F * B) / (E * A - B * D);
	float s = (D * C - A * F) / (D * B - A * E);

	if (Parallel((seg1.point2 - seg1.point1), (seg2.point2 - seg2.point1)))
	{
		return false;
	}

	if ((0.f <= t && t <= 1.f) && (0.f <= s && s <= 1.f))
	{
		if (rt != NULL)
		{
			*rt = t;
		}
		return true;
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

bool Polygon3D::Parallel(const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 test = glm::cross(v1, v2);
	if ((test.x == 0) && (test.y == 0) && (test.z == 0))
	{
		return true;
	}
	return false;
}

float Polygon3D::CalculateSquaredDistance(const glm::vec3& mVec)
{
	return ((mVec.x * mVec.x) + (mVec.y * mVec.y) + (mVec.z * mVec.z));
}