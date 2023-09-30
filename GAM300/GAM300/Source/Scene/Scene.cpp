/*!***************************************************************************************
\file			Scene.cpp
\project
\author

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the definitions of the following:
	1. Scene

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "Scene/Scene.h"


Scene::Scene(const std::string& _filepath)
{
	// Save scene name
	filePath = _filepath;
	sceneName = filePath.stem().string();

	//If a filepath was given
	if (!_filepath.empty())
	{
		//Deserialize
	}
}

Scene::Scene(Scene& rhs) : sceneName{ rhs.sceneName }
{
	CloneHelper(rhs, AllObjectTypes());
}


void Scene::ClearBuffer()
{
	for (Entity* pEntity : entitiesDeletionBuffer)
	{
		layer.erase(std::find(layer.begin(), layer.end(), pEntity->euid));
		entities.erase(*pEntity);
	}
	entitiesDeletionBuffer.clear();
	//Destroy components
	ClearBufferHelper(*this);
}


