#pragma once

#include "glm/glm.hpp"

class Segment2D
{
public:
    Segment2D(const glm::vec2& p1, const glm::vec2& p2)
        : point1(p1), point2(p2) {
        return;
    }

    glm::vec2 point1;
    glm::vec2 point2;
};