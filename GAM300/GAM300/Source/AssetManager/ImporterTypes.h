#pragma once

#include "Utilities/GUID.h"
#include <Properties.h>

struct DefaultImporter : property::base
{
	Engine::HexID guid;
	DefaultImporter(Engine::HexID _guid = {}) :guid{ _guid } {}

	property_vtable();
};


property_begin_name(DefaultImporter, "") {
	property_var(guid).Name("guid"),
} property_vend_h(DefaultImporter)

struct ModelImporter : DefaultImporter
{
	ModelImporter (Engine::HexID _guid = {}) : DefaultImporter::DefaultImporter(_guid){}
	std::vector<Engine::GUID<MeshAsset>> meshes{};
	std::vector<Engine::GUID<MaterialAsset>> materials{};
	std::vector<Engine::GUID<AnimationAsset>> animations{};
	std::unordered_map<std::string, glm::vec2> animationStates{};

	property_vtable();
};

property_begin_name(ModelImporter, "ModelImporter") 
{
	property_parent(DefaultImporter),
	property_var(meshes),
	property_var(materials),
	property_var(animations),
} property_vend_h(ModelImporter)