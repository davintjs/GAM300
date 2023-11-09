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

#include "AssetManager/AssetTypes.h"

// Bean: These are TEMPORARY
#include "../../Compiler/Mesh.h"
#include "Graphics/AnimationManager.h"

// The geom components that are extracted from the aiScene which contains the fbx/obj data
// essentially only a portion of the data extracted from the file is needed
struct GeomComponents
{
	std::vector<Geom_Mesh> meshes{};	// Individual meshes in the model, which also contains its individual vertices and indices
	std::vector<Material> materials{};	// Total materials of the WHOLE model (One mesh uses one material only)
	std::vector<Animation> animations{};// The animations contained on this model
};

struct ModelComponents
{
	std::vector<MeshAsset> meshes;		// Individual meshes in the model in engine terms, which also contains its individual vertices and indices
	std::vector<Material> materials;	// Total materials of the WHOLE model (One mesh uses one material only)
	std::vector<Animation> animations{};// The animations contained on this model
};

#endif // !MODELCLASSANDSTRUCT_H
