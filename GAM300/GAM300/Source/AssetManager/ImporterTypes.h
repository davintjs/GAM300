#pragma once

#include "Utilities/GUID.h"
#include <Properties.h>
#include <unordered_map>
#include <vector>
#include <filesystem>

enum AssetState
{
	ASSET_LOADED,
	ASSET_UPDATED,
	ASSET_UNLOADED
};

struct DefaultImporter : property::base
{
	Engine::HexID guid;
	DefaultImporter(Engine::HexID _guid = {}) :guid{ _guid } {}

	property_vtable();
};


property_begin_name(DefaultImporter, "") {
	property_var(guid).Name("guid"),
} property_vend_h(DefaultImporter)

struct anim_state {
		std::string label;
		Vector2 min_max; //x for min , y for max

		anim_state() { label = "New state"; min_max = Vector2(); }
		anim_state(std::string _label, Vector2 vec2) : label(_label), min_max(vec2) {}
};

struct ModelImporter : DefaultImporter
{
	ModelImporter(Engine::HexID _guid = {}) : DefaultImporter::DefaultImporter(_guid) { scale_factor = 1.f; }
	std::vector<Engine::GUID<MeshAsset>> meshes{};
	std::vector<Engine::GUID<MaterialAsset>> materials{};
	std::vector<Engine::GUID<AnimationAsset>> animations{};
	std::vector<anim_state>animationStates{};

	float scale_factor;

	//std::unordered_map<std::string, glm::vec2> animationStates{};

	property_vtable();
};

property_begin_name(ModelImporter, "ModelImporter") 
{
	property_parent(DefaultImporter),
	property_var(meshes),
	property_var(materials),
	property_var(animations),
	//property_var(scale_factor),
} property_vend_h(ModelImporter)

template <typename T>
using AssetsBuffer = std::vector<std::pair<AssetState, T*>>;


template <typename T>
constexpr auto Importer()
{
	if constexpr (std::is_same_v<T, ModelAsset>)
	{
		return ModelImporter();
	}
	else
	{
		return DefaultImporter();
	}
}

template <typename T>
using AssetImporter = decltype(Importer<T>());
//
template <typename T>
using ImporterTable = std::unordered_map<std::filesystem::path, AssetImporter<T>>;