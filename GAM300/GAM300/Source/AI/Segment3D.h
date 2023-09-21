#pragma once

#include "glm/glm.hpp"

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