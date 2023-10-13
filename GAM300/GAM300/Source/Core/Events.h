/*!***************************************************************************************
\file			Events.h
\project
\author			Zacharie Hong
				Sean Ngo

\par			Course: GAM300
\par			Section:
\date			30/09/2023

\brief
	This file contains the declarations of all event structures in the engine.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef EVENTS_H
#define EVENTS_H

#include "Utilities/UUID.h"
struct Entity;
struct Scene;
struct Script;
struct Field;
struct Rigidbody;

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
	const char** arr = nullptr;
	size_t count = 0;
};

template <typename T>
struct ObjectCreatedEvent : IEvent
{
	ObjectCreatedEvent(T* _pObject) : pObject{ _pObject } {}
	T* pObject;
};

template <typename T>
struct ObjectDestroyedEvent : IEvent
{
	ObjectDestroyedEvent(T* _pObject) : pObject{ _pObject } {}
	T* pObject;
};

struct ScriptSetFieldEvent : IEvent
{
	ScriptSetFieldEvent(Script& _script, const char* _fieldName, Field& _field) : script{ _script }, fieldName{ _fieldName }, field{ _field }{}
	Script& script;
	const char* fieldName;
	Field& field;
};

struct ScriptGetFieldEvent : IEvent
{
	ScriptGetFieldEvent(Script& _script, const char* _fieldName, Field& _field) : script{ _script }, fieldName{ _fieldName }, field{ _field }{}
	Script& script;
	const char* fieldName;
	Field& field;
};

struct ScriptGetFieldNamesEvent : IEvent
{
	ScriptGetFieldNamesEvent(Script& _script) : script{ _script } {}
	Script& script;
	const char** pStart{nullptr};
	size_t count{0};
};

struct GetAssetEvent: IEvent
{
	GetAssetEvent(const std::string& _fileName) : fileName{ _fileName } {}
	const std::string& fileName;
	std::string guid;
};

struct EditorWindowEvent : IEvent
{
	EditorWindowEvent() {};
	bool isHovered = false;
	bool isFocused = false;
};
struct ContactAddedEvent : IEvent
{
	ContactAddedEvent() : rb1{ nullptr }, rb2{ nullptr }{}
	Rigidbody* rb1;
	Rigidbody* rb2;

};
struct ContactRemovedEvent : IEvent
{
	ContactRemovedEvent() : rb1{ nullptr }, rb2{ nullptr }{}
	Rigidbody* rb1;
	Rigidbody* rb2;
};


//
//template <typename T>
//struct ComponentDeleteEvent : IEvent
//{
//	ComponentDeleteEvent(T& _component) : component{_component}{}
//	T& component;
//};

#endif //!EVENTS_H