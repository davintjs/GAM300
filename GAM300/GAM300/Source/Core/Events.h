/*!***************************************************************************************
\file			events.h
\project
\author			Zacharie Hong
				Sean Ngo

\par			Course: GAM200
\par			Section:
\date			15/02/2023

\brief
	This file contains the declarations of all event structures in the engine.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EVENTS_H
#define EVENTS_H

#include "Scene/Scene.h"
#include "Core/FileTypes.h"

struct IEvent
{
protected:
	virtual ~IEvent() {};
};

struct StartPreviewEvent : IEvent
{
	StartPreviewEvent(){}
};

struct StopPreviewEvent : IEvent
{
	StopPreviewEvent(){}
};

struct QuitEngineEvent : IEvent
{
	QuitEngineEvent() {};
};

struct SceneChangingEvent : IEvent
{
	SceneChangingEvent(Scene& _scene) : scene{ _scene } {}
	Scene& scene;
};

template <typename T>
struct ReflectComponentEvent : IEvent
{
	ReflectComponentEvent(T& _component) : component{ _component } {}
	T& component;
};

struct ReflectEntityEvent : IEvent
{
	ReflectEntityEvent(Entity& _entity) : entity{ _entity} {}
	Entity& entity;
};


template <FileType FTYPE>
struct FileModifiedEvent : IEvent 
{
	FileModifiedEvent(const std::string& _filePath) : filePath{ _filePath }{};
	std::string filePath;
	FileState fileState;
};


//struct GetCurrentSceneEvent
//{
//	GetCurrentSceneEvent(Scene& _scene) :scene{ _scene } {};
//	Scene& scene;
//};

//template <typename T>
//struct ComponentAddEvent : IEvent
//{
//	ComponentAddEvent(const Entity& _entity, T*& _componentContainer, UUID _uuid = UUID()) :
//		entity{ _entity }, componentContainer{ _componentContainer }, uuid{_uuid} {}
//	Entity& entity;
//	T*& componentContainer;
//	UUID uuid;
//};
//
//template <typename T>
//struct ComponentDeleteEvent : IEvent
//{
//	ComponentDeleteEvent(T& _component) : component{_component}{}
//	T& component;
//};

#endif //!EVENTS_H