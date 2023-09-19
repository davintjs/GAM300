#pragma once

#include "glm/glm.hpp"

class Line3D
{
public:
    Line3D(const glm::vec3& p, const glm::vec3& v) : point(p), vector(v) { }
    glm::vec3 lerp(const float t) const { return (point + t * vector); }

    glm::vec3 point;
    glm::vec3 vector;
};