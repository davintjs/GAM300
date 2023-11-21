/*!***************************************************************************************
\file			Scene.h
\project
\author			Zacharie Hong

\par			Course: GAM300
\date			10/08/2023

\brief
	This file defines non template functions used by Scene which functions as a ECS

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
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

Scene& Scene::operator=(Scene& rhs)
{
	CloneHelper(rhs, AllObjectTypes());
	return *this;
}


Entity& Scene::Clone(Entity& source)
{
	ReferencesTable references;
	Transform& sourceTrans{ Get<Transform>(source) };
	Entity& dest = StoreTransformHierarchy(references, source.EUID());
	if (sourceTrans.parent)
	{
		Transform& parent{ Get<Transform>(sourceTrans.parent) };
		parent.child.push_back(dest.EUID());
		Transform& destTrans{ Get<Transform>(dest) };
		destTrans.parent = parent.EUID();
	}
	LinkReferences(references, AllComponentTypes());
	return dest;
}



Entity& Scene::StoreTransformHierarchy(ReferencesTable& storage, Engine::UUID entityID)
{
	Entity& key = Get<Entity>(entityID);
	Entity& val = *Add<Entity>();
	storage[GetType::E<Entity>()][key] = val;
	Transform& transform{ Get<Transform>(entityID) };
	StoreComponentHierarchy(storage,entityID,val.EUID(),AllComponentTypes());
	//Create map entry
	for (Engine::UUID euid : transform.child)
	{
		StoreTransformHierarchy(storage, euid);
	}
	return val;
}

void Scene::ClearBuffer()
{
	for (Entity* pEntity : entitiesDeletionBuffer)
	{
		layer.remove(pEntity->euid);
		entities.erase(*pEntity);
	}
	entitiesDeletionBuffer.clear();
	//Destroy components
	ClearBufferHelper(*this);
}

