/*!***************************************************************************************
\file			ModelClassAndStruct.h
\project
\author         Sean Ngo

\par			Course: GAM300
\date           31/10/2023

\brief
    This file contains the declarations of the following:
    1. Models
    2. Mesh, Material, Animation, Texture

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef MODELCLASSANDSTRUCT_H
#define MODELCLASSANDSTRUCT_H

#include <vector>

#include "GeometryMesh.h"
#include "Graphics/Animation/Animation.h"

#define MAX_BONE_INFLUENCE 4
struct ModelVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 textureCords;
	glm::ivec4 color;

	// Animation Related Properties
	int boneIDs[MAX_BONE_INFLUENCE];
	float weights[MAX_BONE_INFLUENCE];
};

#endif // !MODELCLASSANDSTRUCT_H
