#pragma once

#include "glm/glm.hpp"

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