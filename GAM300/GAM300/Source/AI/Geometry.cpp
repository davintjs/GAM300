/*!***************************************************************************************
\file			Geometry.cpp
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
    This file contains the definitions of the following:
    1. Triangle3D class
        a. A 3D triangle class used for navigation mesh
        b. Getter functions
        c. Addition of neighbouring triangles
        d. Point check in triangle

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"

#include "Geometry.h"

bool Segment3D::PointLiesOnLine(const glm::vec3& mPoint)
{
	return false;
}

Triangle3D::Triangle3D(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3)
{
    mPoints[0] = p1;
    mPoints[1] = p2;
    mPoints[2] = p3;
    mMidPoint = (p1 + p2 + p3) / 3.f;
    mNormal = glm::cross((p2 - p1), (p3 - p1));
}

Triangle3D::~Triangle3D()
{

}

std::vector<Triangle3D*> Triangle3D::GetNeighbours()
{
    return mNeighbours;
}

const glm::vec3 Triangle3D::GetMidPoint() const
{
    return mMidPoint;
}
const glm::vec3 Triangle3D::GetNormal() const
{
    return mNormal;
}

bool Triangle3D::isNeighbour(const Triangle3D& mRHS)
{
	if (this->ContainsPoint(mRHS[0]) && this->ContainsPoint(mRHS[1]) ||
		this->ContainsPoint(mRHS[1]) && this->ContainsPoint(mRHS[2]) ||
		this->ContainsPoint(mRHS[2]) && this->ContainsPoint(mRHS[0]) ||
		mRHS.ContainsPoint(mPoints[0]) && mRHS.ContainsPoint(mPoints[1]) ||
		mRHS.ContainsPoint(mPoints[1]) && mRHS.ContainsPoint(mPoints[2]) ||
		mRHS.ContainsPoint(mPoints[2]) && mRHS.ContainsPoint(mPoints[0]))
	{
		return true;
	}
	return false;
}

bool Triangle3D::ContainsPoint(const glm::vec3& mPoint) const
{
    glm::vec3 A = this->mPoints[0];
    glm::vec3 B = this->mPoints[1];
    glm::vec3 C = this->mPoints[2];

    glm::vec3 p_Normal = glm::cross((B - A), (C - A));

    if (glm::dot((mPoint - A), glm::cross(p_Normal, (B - A))) < 0.f)
    {
        return false;
    }
    if (glm::dot((mPoint - B), glm::cross(p_Normal, (C - B))) < 0.f)
    {
        return false;
    }
    if (glm::dot((mPoint - C), glm::cross(p_Normal, (A - C))) < 0.f)
    {
        return false;
    }
    return true;
}

void Triangle3D::AddNeighbour(Triangle3D* mTri)
{
    mNeighbours.push_back(mTri);
}

const int Triangle3D::GetID()
{
    return mTriID;
}

void Triangle3D::SetTriID(int id)
{
    mTriID = id;
}

bool Intersects(const Segment2D& seg1, const Segment2D& seg2, float* rt)
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

bool Intersects(const Segment3D& seg1, const Segment3D& seg2, float* rt)
{
	float A = seg1.point2.x - seg1.point1.x;
	float B = seg2.point1.x - seg2.point2.x;
	float C = seg2.point1.x - seg1.point1.x;

	float D = seg1.point2.z - seg1.point1.z;
	float E = seg2.point1.z - seg2.point2.z;
	float F = seg2.point1.z - seg1.point1.z;

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

bool Intersects(const Line3D& line, const Plane3D& plane, float* rt)
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

	//float Mtv = MdotN + (t * VdotN);
	//float test = Mtv + plane[3];
	//if (test != 0.f)
	//{
	//	return false;
	//}

	return true;

}

bool Parallel(const glm::vec3& v1, const glm::vec3& v2)
{
	glm::vec3 test = glm::cross(v1, v2);
	if ((test.x == 0) && (test.y == 0) && (test.z == 0))
	{
		return true;
	}
	return false;
}