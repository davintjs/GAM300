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


void Scene::Clone(Entity& source)
{
	ReferencesTable references;
	StoreTransformHierarchy(references, source.EUID());
	LinkReferences(references, AllComponentTypes());
}



void Scene::StoreTransformHierarchy(ReferencesTable& storage, Engine::UUID entityID)
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
}



//ReferencesTable Scene::PreClone(Entity& target)
//{
//
//	static char buffer[2048]{};
//	for (Script* pScript : GetMulti<Script>(target))
//	{
//		ScriptGetFieldNamesEvent getFieldNamesEvent{ *pScript };
//		EVENTS.Publish(&getFieldNamesEvent);
//		for (size_t i = 0; i < getFieldNamesEvent.count; ++i)
//		{
//			const char* fieldName = getFieldNamesEvent.pStart[i];
//			Field field{ AllFieldTypes::Size(),2048, buffer };
//			ScriptGetFieldEvent getFieldEvent{ *pScript,fieldName,field };
//			EVENTS.Publish(&getFieldEvent);
//			//Objects
//			if (field.fType >= FieldTypes::Size())
//			{
//				Handle& handle{ field.Get<Handle>() };
//				if (references.contains(handle))
//				{
//					//Set to internal linkage in game object
//					handle = references[handle];
//				}
//				ScriptSetFieldEvent setFieldEvent{ *pScript,fieldName,field };
//				EVENTS.Publish(&setFieldEvent);
//			}
//
//		}
//	}
//
//	static char buffer[2048]{};
//}

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

