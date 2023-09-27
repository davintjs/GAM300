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

#include "Utilities/UUID.h"
struct Entity;
struct Scene;
struct Script;

struct IEvent
{
protected:
	virtual ~IEvent() {};
};

struct QuitEngineEvent : IEvent
{
	QuitEngineEvent() {};
};

struct CreateSceneEvent : IEvent
{
	CreateSceneEvent(Scene* _scene) : scene(_scene) {}
	Scene* scene;
};

struct LoadSceneEvent : IEvent
{
	LoadSceneEvent(const std::string& _filePath) :filePath{ _filePath } {}
	std::string filePath;
};

struct SaveSceneEvent : IEvent
{
	SaveSceneEvent() {};
	SaveSceneEvent(const std::string& _filePath) :filePath{ _filePath } {}
	std::string filePath;
};

struct IsNewSceneEvent : IEvent
{
	IsNewSceneEvent() : data(false) {}
	bool data;
};

struct SceneChangingEvent : IEvent 
{
	SceneChangingEvent(Scene& _scene) : scene(_scene) {}
	Scene& scene;
};

template <size_t FTYPE>
struct FileTypeModifiedEvent : IEvent 
{
	FileTypeModifiedEvent(const wchar_t* _fileName, size_t _fileState) : fileName{ _fileName }, fileState{_fileState}{}
	const wchar_t* fileName;
	size_t fileState;
};

struct FileModifiedEvent : IEvent
{
	FileModifiedEvent(const wchar_t* _filePath, size_t _fileState) : filePath{ _filePath }, fileState{ _fileState }{}
	const wchar_t* filePath;
	size_t fileState;
};

struct SceneStartEvent : IEvent{};

struct SceneStopEvent : IEvent {};

struct SceneCleanupEvent : IEvent {};

struct SelectedEntityEvent : IEvent
{
	SelectedEntityEvent(Entity* _pEntity) : pEntity{ _pEntity }{}
	Entity* pEntity;
};

struct GetScriptNamesEvent : IEvent
{
	GetScriptNamesEvent() {};
	const char** arr;
	size_t count;
};

template <typename T>
struct ObjectCreatedEvent : IEvent
{
	ObjectCreatedEvent(T* _pObject) : pObject{ _pObject } {}
	T* pObject;
};

struct ScriptSetFieldEvent : IEvent
{
	ScriptSetFieldEvent(Script& _script, const char* _fieldName) : script{ _script }, fieldName{ _fieldName } {}
	Script& script;
	const char* fieldName;
};


//
//template <typename T>
//struct ComponentDeleteEvent : IEvent
//{
//	ComponentDeleteEvent(T& _component) : component{_component}{}
//	T& component;
//};

#endif //!EVENTS_H