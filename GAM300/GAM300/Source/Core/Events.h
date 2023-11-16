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

#include "EventInterface.h"

#include "Utilities/UUID.h"
#include <glm/vec2.hpp>
#include <filesystem>
#include <AssetManager/AssetTypes.h>
#include <AssetManager/ImporterTypes.h>

struct Entity;
struct Scene;
struct Script;
struct Field;
struct Rigidbody;
struct ImGuiTextBuffer;

namespace fs = std::filesystem;

struct ApplicationExitEvent : IEvent
{
	ApplicationExitEvent() {};
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

struct ScenePostCleanupEvent : IEvent {};

struct SelectedEntityEvent : IEvent
{
	SelectedEntityEvent(Entity* _pEntity) : pEntity{ _pEntity }{}
	Entity* pEntity;
};

struct GetSelectedEntityEvent : IEvent
{
	GetSelectedEntityEvent() {}
	Entity* pEntity;
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

#pragma region Asset Manager

template <typename T>
struct GetAssetEvent: IEvent
{
	GetAssetEvent(const fs::path& _filePath) : filePath{ _filePath } {}
	const fs::path& filePath;
	Engine::GUID<T> guid;
	Asset* asset;
};

template <typename T>
struct GetAssetByGUIDEvent : IEvent
{
	GetAssetByGUIDEvent(Engine::GUID<T> _guid) : guid{ _guid } {}
	Engine::GUID<T> guid;
	AssetImporter<T>* importer;
	T* asset;
};

template <typename T>
struct AddSubAssetEvent : IEvent
{
	AddSubAssetEvent(T& _asset, Engine::GUID<T> _guid = Engine::GUID<T>{}) : asset{ _asset }, guid{ _guid } {}
	T& asset;
	Engine::GUID<T> guid;
};

template <typename T>
struct GetFilePathGenericEvent : IEvent
{
	GetFilePathGenericEvent(const Engine::GUID<T>& _guid) : guid{ _guid } {}
	const Engine::GUID<T>& guid;
	fs::path filePath;
};

template <typename AssetType>
struct GetFilePathEvent : IEvent
{
	GetFilePathEvent(const Engine::GUID<AssetType>& _guid) : guid{ _guid } {}
	const Engine::GUID<AssetType>& guid;
	fs::path filePath;
};


template <typename AssetType>
struct AssetLoadedEvent : IEvent
{
	AssetLoadedEvent(const fs::path& _assetPath, const Engine::GUID<AssetType>& _guid, const AssetType& _asset)
		: assetPath{ _assetPath }, guid{ _guid }, asset{ _asset } {}
	const fs::path& assetPath;
	const Engine::GUID<AssetType>& guid;
	const AssetType& asset;
};

template <typename AssetType>
struct AssetUpdatedEvent : IEvent
{
	AssetUpdatedEvent(const fs::path& _assetPath, const Engine::GUID<AssetType>& _guid, const AssetType& _asset)
		: assetPath{ _assetPath }, guid{ _guid }, asset{ _asset } {}
	const fs::path& assetPath;
	const Engine::GUID<AssetType>& guid;
	const AssetType& asset;
};

template <typename AssetType>
struct AssetUnloadedEvent : IEvent
{
	AssetUnloadedEvent(const fs::path& _assetPath, const Engine::GUID<AssetType>& _guid) :
		assetPath{ _assetPath }, guid{ _guid } {}
	const fs::path& assetPath;
	const Engine::GUID<AssetType>& guid;
};

struct DropAssetsEvent : IEvent
{
	DropAssetsEvent(const int& _pathCount, const fs::path* _paths) : pathCount{ _pathCount }, paths{ _paths } {}
	int pathCount;
	const fs::path* paths;
};

template <typename AssetType>
struct GetAssetsEvent : IEvent
{
	AssetsTable<AssetType>* pAssets{};
};

#pragma endregion

struct ContactAddedEvent : IEvent
{

	ContactAddedEvent() : rb1{ nullptr }, rb2{ nullptr } {}
	Rigidbody* rb1;
	Rigidbody* rb2;

};
struct ContactRemovedEvent : IEvent
{
	ContactRemovedEvent() : rb1{ nullptr }, rb2{ nullptr } {}
	Rigidbody* rb1;
	Rigidbody* rb2;
};

struct TriggerEnterEvent : IEvent
{
	TriggerEnterEvent() : rb1{ nullptr }, rb2{ nullptr }{}
	Rigidbody* rb1;
	Rigidbody* rb2;
};
struct TriggerRemoveEvent : IEvent
{
	TriggerRemoveEvent() : rb1{ nullptr }, rb2{ nullptr }{}
	Rigidbody* rb1;
	Rigidbody* rb2;
};

#pragma region EDITOR STUFF

struct EditorWindowEvent : IEvent
{
	EditorWindowEvent(const std::string& _name) : name{ _name } {};
	bool isOpened = false;
	bool isHovered = false;
	bool isFocused = false;
	std::string name;
};

struct EditorPanCameraEvent : IEvent
{
	EditorPanCameraEvent(const bool& _pan) : isPanning{ _pan } {}
	bool isPanning = false;
};

struct EditorUpdateSceneGeometryEvent : IEvent
{
	EditorUpdateSceneGeometryEvent(const glm::vec2& _position, const glm::vec2& _dimension) : position{ _position }, dimension{ _dimension } {}
	glm::vec2 position;
	glm::vec2 dimension;
};

struct EditorSetGameCameraEvent : IEvent
{
	EditorSetGameCameraEvent(const unsigned int& _targetDisplay) : targetDisplay{ _targetDisplay } {}
	unsigned int targetDisplay;
};

struct EditorGetCurrentDirectory : IEvent
{
	EditorGetCurrentDirectory() {}
	std::string path;
};

#pragma endregion

//
//template <typename T>
//struct ComponentDeleteEvent : IEvent
//{
//	ComponentDeleteEvent(T& _component) : component{_component}{}
//	T& component;
//};

#endif //!EVENTS_H