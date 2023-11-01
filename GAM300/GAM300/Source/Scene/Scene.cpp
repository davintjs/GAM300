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

//template<typename T, typename... Ts>
//struct CloneEntityHelper
//{
//	constexpr CloneEntityHelper(TemplatePack<T, Ts...>);
//	CloneEntityHelper<>
//};

//using CloneEntity = decltypCloneEntityHelper;

//template<typename T, typename... Ts>
//void Scene::CloneHelper()
//{
//
//}
//
//void Scene::Clone(Entity& entity)
//{
//
//}

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
	CloneHelper(source, AllComponentTypes());
}

//template <typename T, typename... Ts>
//void Scene::StoreComponentHierarchy(ReferencesTable& storage, Engine::UUID transformID)
//{
//	Transform& transform{ Get<Transform>(transformID) };
//	//Create map entry
//	storage[transform];
//	for (Engine::UUID euid : transform.child)
//	{
//		StoreTransformHierarchy(storage, transformID);
//	}
//}
//
//void Scene::StoreTransformHierarchy(ReferencesTable& storage, Engine::UUID transformID)
//{
//	Transform& transform{ Get<Transform>(transformID) };
//	//Create map entry
//	storage[transform];
//	for (Engine::UUID euid : transform.child)
//	{
//		StoreTransformHierarchy(storage, transformID);
//	}
//}

//ReferencesTable Scene::PreClone(Entity& target)
//{
//	ReferencesTable references;
//	//StoreTransformHierarchy(references, target.EUID());
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

//template <typename T, typename... Ts>
//Handle CloneHelper(Scene& scene, Engine::UUID euid, Engine::UUID parentID = 0, Engine::UUID newID)
//{
//	T* newObject{ nullptr };
//	if constexpr (SingleObjectTypes::Has<T>())
//	{
//		if constexpr (std::is_same<T, Entity>)
//		{
//			newObject = scene.Add<T>(newID);
//			Transform& newTrans = scene.Get<Transform>(newID);
//			newTrans.parent = parentID;
//			Transform& trans = scene.Get<Transform>(euid);
//			newTrans.rotation = trans.rotation;
//			newTrans.position = trans.position;
//			newTrans.scale = trans.scale;
//			//Clone child
//			for (auto child : object.child)
//			{
//				Engine::UUID childID = CloneHelper<T,Ts...>(scene, child, newID);
//				newTrans.child.push_back(childID);
//			}
//		}
//		if constexpr (!std::is_same<T, Transform>())
//		{
//			CloneHelper(scene,newObject,scene.Get<)
//		}
//	}
//	else
//	{
//
//	}
//
//	if constexpr (sizeof(Ts...) != 0)
//	{
//		CloneHelper<Ts...>(euid);
//	}
//	else
//	{
//		return *newObject;
//	}
//}
//
//template <typename... Ts>
//void CloneHelper(Scene& scene,Engine::UUID euid,TemplatePack<Ts...>)
//{
//	return CloneHelper<Ts...>(scene, euid, scene.Get<Transform>(euid).parent);
//}