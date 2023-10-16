#pragma once

#include "Core/SystemInterface.h"

#include <map>
#include <string>

#include "Utilities/UUID.h"


#define MAX_PHYSICS_LAYERS 32
#define IDENTIFIERS IdentifiersManager::Instance()

struct Layer
{
	std::string name;
};

using Tags = std::map<std::string, Engine::UUID>;
using Layers = std::vector<Layer>;

SINGLETON(IdentifiersManager)
{
public:
	//Const size because bitwise
	Layer physicsLayers[MAX_PHYSICS_LAYERS];
	void CreateTag(const std::string& tagName);
	void CreateLayer(const std::string & layerName);

	Tags& GetTags();

	void CreateSortingLayer(const std::string & layerName,size_t index);
	void RenameSortingLayer(const std::string & layerName, size_t index);
	void SwapSortingLayers(size_t rhs, size_t lhs);
	void DeleteSortingLayer(size_t index);
private:
	Tags tags;
	Layers sortingLayers;
};