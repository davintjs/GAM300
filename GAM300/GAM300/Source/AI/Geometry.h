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
        crds[0] = A; crds[1] = B; crds[2] = C; crds[3] = D;
    }

    float& operator[](const unsigned int i) { return crds[i]; }
    const float& operator[](const unsigned int i) const { return crds[i]; }

    glm::vec3 normal() const { return glm::vec3(crds[0], crds[1], crds[2]); }

private:
    enum { DIM = 4 };
    float crds[DIM];
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
    std::vector<Triangle3D*> GetNeighbours();
    const glm::vec3 GetMidPoint() const;
    const glm::vec3 GetNormal() const;

    void AddNeighbour(Triangle3D* mTri);

    bool ContainsPoint(const glm::vec3& mPoint) const;

    const int GetID();
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