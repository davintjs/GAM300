#pragma once

#include "glm/glm.hpp"

class BoundingBox3D
{
public:
    // Constructor
    BoundingBox3D() {}
    BoundingBox3D(const glm::vec3& center, const glm::vec3& extents) : mCenter(center), mExtents(extents) { return; }

    glm::vec3 mCenter{};    // Center point
    glm::vec3 mExtents{};   // Center to corner half extents.
};