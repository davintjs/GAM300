/*!***************************************************************************************
\file			BoundingBox.h
\project
\author         Davin Tan

\par			Course: GAM300
\date           28/09/2023

\brief
    This file contains the declarations of the following:
    1. BoundingBox3D class for compression usage in assimp loading

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#pragma once

#include "glm/glm.hpp"

class BoundingBox3D
{
public:
    // Constructor
    BoundingBox3D() 
    {
        mMin.x = FLT_MAX;
        mMin.y = FLT_MAX;
        mMin.z = FLT_MAX;

        mMax.x = -FLT_MAX;
        mMax.y = -FLT_MAX;
        mMax.z = -FLT_MAX;
    }
    BoundingBox3D(const glm::vec3& min, const glm::vec3& max) : mMin(min), mMax(max) { return; }

    glm::vec3 mMin{};    // Center point
    glm::vec3 mMax{};    // Center to corner half extents.
};