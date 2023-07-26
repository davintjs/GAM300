#pragma once

#include "glm/glm.hpp"

class BoundingBox3D
{
public:
    // Constructor
    BoundingBox3D() {}
    BoundingBox3D(const glm::vec3& min, const glm::vec3& max) : mMin(min), mMax(max) { return; }

    glm::vec3 mMin{};    // Center point
    glm::vec3 mMax{};    // Center to corner half extents.
};