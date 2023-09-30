/*!***************************************************************************************
\file			Geometry.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
    This file contains the declarations of the following:
    1. Line3D class
        a. A 3D line class used for navigation mesh
    2. Plane3D class
        a. A 3D plane class used for navigation mesh
    3. Segment2D class
        a. A 2D segment class used for navigation mesh 
    4. Segment3D class
        a. A 3D segment class used for navigation mesh
    5. Triangle3D class
        a. A 3D triangle class used for navigation mesh
        b. Getter functions
        c. Addition of neighbouring triangles
        d. Point check in triangle

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include <vector>
#include "glm/glm.hpp"

class Line3D
{
public:
    Line3D(const glm::vec3& p, const glm::vec3& v) : point(p), vector(v) { }
    glm::vec3 lerp(const float t) const { return (point + t * vector); }

    glm::vec3 point;
    glm::vec3 vector;
};

class Plane3D
{
public:
    Plane3D(const float A = 0, const float B = 0, const float C = 0, const float D = 0)
    {
        mCoords[0] = A; mCoords[1] = B; mCoords[2] = C; mCoords[3] = D;
    }

    float& operator[](const unsigned int i) { return mCoords[i]; }
    const float& operator[](const unsigned int i) const { return mCoords[i]; }

    // Returns the normal of the plane
    glm::vec3 normal() const { return glm::vec3(mCoords[0], mCoords[1], mCoords[2]); }

private:
    enum { DIM = 4 };
    float mCoords[DIM];
};

class Segment2D
{
public:
    Segment2D(const glm::vec2& p1, const glm::vec2& p2)
        : point1(p1), point2(p2)
    {
        return;
    }

    glm::vec2 point1;
    glm::vec2 point2;
};

class Segment3D
{
public:
    // Constructors
    Segment3D() : point1(), point2() { return; }
    Segment3D(const glm::vec3& p1, const glm::vec3& p2)
        : point1(p1), point2(p2)
    {
        return;
    }
    
    // Returns the point after applying t in parametric equation
    glm::vec3 lerp(const float t) const
    {
        return ((1.0f - t) * point1 + t * point2);
    }

    glm::vec3 point1;
    glm::vec3 point2;
};

enum class onList // For pathfinding
{
    NONE,
    OPEN_LIST,
    CLOSED_LIST
};

class Triangle3D
{
public:
    Triangle3D(const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3);
    ~Triangle3D();

    glm::vec3& operator[](unsigned int i) { return mPoints[i]; }
    const glm::vec3& operator[](unsigned int i) const { return mPoints[i]; }

    // Getter functions
    // Returns the neighbours of this triangle
    std::vector<Triangle3D*> GetNeighbours();

    // Returns the midpoint of this triangle
    const glm::vec3 GetMidPoint() const;

    // Returns the normal of this triangle
    const glm::vec3 GetNormal() const;

    // Add neighbour of this triangle
    void AddNeighbour(Triangle3D* mTri);

    // Checks if a point is in this triangle
    bool ContainsPoint(const glm::vec3& mPoint) const;

    // Get the ID of this triangle
    const int GetID();

    // Set the ID of this triangle
    void SetTriID(int id);

private:
    // NavMesh stuff
    glm::vec3 mPoints[3];
    Triangle3D* mParent;
    glm::vec3 mMidPoint;
    glm::vec3 mNormal;
    std::vector<Triangle3D*> mNeighbours;
    int mTriID = 0;

    // Pathfinding stuff
    float mFinalCost = 0.f;
    float mHeuCost = 0.f;
    float mGivenCost = 0.f;
    onList mOnList = onList::NONE;
};