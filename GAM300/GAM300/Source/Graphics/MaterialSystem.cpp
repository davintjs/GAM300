/*!***************************************************************************************
\file			MaterialSystem.cpp
\project
\author			Euan Lim, Joseph Ho, Zacharie Hong

\par			Course: GAM300
\date           11/10/2023

\brief
	This file contains the definitions of Material System that includes:
	1. Functionalities to Serialize Deserialize Material Instances
	2. Make Duplicates of Material Instances 

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "GraphicsHeaders.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"
#include "Utilities/Serializer.h"
#include "Texture/TextureManager.h"


void MaterialSystem::Init()
{
	EVENTS.Subscribe(this, &MaterialSystem::CallbackMaterialAssetLoaded);
	EVENTS.Subscribe(this, &MaterialSystem::CallbackBindTexturesOnSceneLoad);

	//NewMaterialInstance();

	// Creating PBR (Instanced) Material
	createPBR_Instanced();

	// Creating PBR (Non-Instanced) Material
	createPBR_NonInstanced();

	//temp 	
	available_shaders.push_back(Shader("Default", SHADERTYPE::DEFAULT));
	available_shaders.push_back(Shader("PBR", SHADERTYPE::PBR));
	
	// Assign texture ids to the material's textures
	BindAllTextureIDs();
}

void MaterialSystem::Update(float dt)
{
	// Empty by design
}

void MaterialSystem::Exit()
{
	// Empty by design

}

void MaterialSystem::BindTextureIDs(Material_instance& _instance)
{
	_instance.textureID = TextureManager.GetTexture(_instance.albedoTexture);
	_instance.normalID = TextureManager.GetTexture(_instance.normalMap);
	_instance.metallicID = TextureManager.GetTexture(_instance.metallicTexture);
	_instance.roughnessID = TextureManager.GetTexture(_instance.roughnessTexture);
	_instance.ambientID = TextureManager.GetTexture(_instance.aoTexture);
	_instance.emissiveID = TextureManager.GetTexture(_instance.emissionTexture);
}

void MaterialSystem::BindAllTextureIDs()
{
	for (auto& [guid, instance] : _allMaterialInstances)
	{
		instance.textureID = TextureManager.GetTexture(instance.albedoTexture);
		instance.normalID = TextureManager.GetTexture(instance.normalMap);
		instance.metallicID = TextureManager.GetTexture(instance.metallicTexture);
		instance.roughnessID = TextureManager.GetTexture(instance.roughnessTexture);
		instance.ambientID = TextureManager.GetTexture(instance.aoTexture);
		instance.emissiveID = TextureManager.GetTexture(instance.emissionTexture);
	}
}

void MaterialSystem::createPBR_Instanced()
{
	//Test material instances
	//Material_instance emissionMat;
	//emissionMat.name = "Emission material";
	//emissionMat.albedoColour = glm::vec4(0.f, 255.f, 255.f, 255.f);
	//emissionMat.metallicConstant = 0.3f;
	//emissionMat.roughnessConstant = 0.2f;
	//emissionMat.aoConstant = 1.f;

	//Material_instance blackSurfaceMat;
	//blackSurfaceMat.name = "Black Surface material";
	//blackSurfaceMat.albedoColour = glm::vec4(0.f, 0.f, 0.f, 255.f);
	//blackSurfaceMat.metallicConstant = 0.2f;
	//blackSurfaceMat.roughnessConstant = 1.f;
	//blackSurfaceMat.aoConstant = 1.f;

	//Material_instance darkBlueMat;
	//darkBlueMat.name = "Dark Blue material";
	//darkBlueMat.albedoColour = glm::vec4(0.f, 0.f, 255.f, 255.f);
	//darkBlueMat.metallicConstant = 0.4f;
	//darkBlueMat.roughnessConstant = 0.5f;
	//darkBlueMat.aoConstant = 1.f;

	//Serialize(darkBlueMat);
	//Material_instance test;
	//GetAssetsEvent<MaterialAsset> e;
	//EVENTS.Publish(&e);
	////for (auto& asset : *e.pAssets)
	////{
	////	asset.second.mFilePath();
	////}
	//fs::path path{ "Assets/Dark Blue material.material" };
	//Deserialize(test,path);

	//_material[SHADERTYPE::PBR].push_back(emissionMat);
	//_material[SHADERTYPE::PBR].push_back(blackSurfaceMat);
	//_material[SHADERTYPE::PBR].push_back(darkBlueMat);

	//deleteInstance(blackSurfaceMat);
}

void MaterialSystem::AddMaterial(const Material_instance& new_mat) {
	//_material[(SHADERTYPE)new_mat.shaderType].push_back(new_mat);
}


void MaterialSystem::createPBR_NonInstanced()
{

	//Material_Types.push_back(SHADERTYPE::DEFAULT);


}

Engine::GUID<MaterialAsset> MaterialSystem::NewMaterialInstance(std::string _name)
{
	Material_instance defaultMaterial;
	defaultMaterial.name = _name;
	Engine::GUID<MaterialAsset> guid;
	_allMaterialInstances[guid] = defaultMaterial;
	return guid;
}

void MaterialSystem::deleteInstance(Engine::GUID<MaterialAsset>& matGUID)
{
	//Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	//// Swap them out to default material before deleting the instance
	//for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
	//{
	//	if (renderer.material_ptr == NULL)
	//	{
	//		continue;
	//	}

	//	if (renderer.material_ptr->matGUID == matInstance.matGUID)// change to matInstanceName
	//	{
	//		renderer.material_ptr = &(_material[(SHADERTYPE)matInstance.shaderType][0]);
	//	}
	//}

	//// Deleting the material instance
	//for (std::vector<Material_instance>::iterator iter(_material[(SHADERTYPE)matInstance.shaderType].begin());
	//	iter != _material[(SHADERTYPE)matInstance.shaderType].end();
	//	++iter)
	//{
	//	if ((*iter).name == matInstance.name)// change to matInstanceName
	//	{
	//		iter = _material[(SHADERTYPE)matInstance.shaderType].erase(iter);
	//	}


	//}

	_allMaterialInstances.erase(matGUID);

}


void MaterialSystem::LoadMaterial(const MaterialAsset& _materialAsset, const Engine::GUID<MaterialAsset>& _guid)
{

	//_allMaterialInstances[_guid](Deserialize(_materialAsset.mFilePath));
	Deserialize(_allMaterialInstances[_guid], _materialAsset.mFilePath);

}

void MaterialSystem::CallbackMaterialAssetLoaded(AssetLoadedEvent<MaterialAsset>* pEvent)
{

	LoadMaterial(pEvent->asset, pEvent->asset.importer->guid);

}

void MaterialSystem::CallbackBindTexturesOnSceneLoad(LoadSceneEvent* pEvent)
{
	BindAllTextureIDs();
}

Engine::GUID<MaterialAsset> MaterialSystem::InstantiateRuntimeMaterial(Material_instance& mat)
{
	Material_instance tmpMat;
	tmpMat = mat;
	auto ret = _runtimeMaterialInstances.insert(std::make_pair(Engine::GUID<MaterialAsset>(), tmpMat));
	auto it = ret.first;
	return it->first;
}

Material_instance& MaterialSystem::getMaterialInstance(Engine::GUID<MaterialAsset> matGUID)
{
	static Material_instance defaultInstance;

	auto iter = _allMaterialInstances.find(matGUID);

	if (iter != _allMaterialInstances.end())
	{
		return iter->second;
	}

	iter = _runtimeMaterialInstances.find(matGUID);

	if (iter != _runtimeMaterialInstances.end())
	{
		return iter->second;
	}

	return defaultInstance;

}

Material_instance::Material_instance()
{
	shaderType = (int)SHADERTYPE::PBR;
	name = "Default Material";
	albedoColour = Vector4(1.f, 1.f, 1.f, 1.f);
	metallicConstant = 1.f;
	roughnessConstant = 1.f;
	aoConstant = 1.f;
	emissionConstant = 1.f;

	albedoTexture = 0;
	normalMap = 0;
	metallicTexture = 0;
	roughnessTexture = 0;
	aoTexture = 0;
	emissionTexture = 0;
}

// This is for Editor
Material_instance::Material_instance(const Material_instance& other)
{
	// Copy each member variable from 'other' to 'this'
	shaderType = other.shaderType;
	name = other.name;
	albedoColour = other.albedoColour;
	metallicConstant = other.metallicConstant;
	roughnessConstant = other.roughnessConstant;
	aoConstant = other.aoConstant;
	emissionConstant = other.emissionConstant;

	albedoTexture = other.albedoTexture;
	normalMap = other.normalMap;
	metallicTexture = other.metallicTexture;
	roughnessTexture = other.roughnessTexture;
	aoTexture = other.aoTexture;
	emissionTexture = other.emissionTexture;
}

Material_instance& Material_instance::operator = (const Material_instance& rhs)
{
	albedoColour = rhs.albedoColour;
	metallicConstant = rhs.metallicConstant;
	roughnessConstant = rhs.roughnessConstant;
	aoConstant = rhs.aoConstant;
	emissionConstant = rhs.emissionConstant;
	albedoTexture = rhs.albedoTexture;
	normalMap = rhs.normalMap;
	metallicTexture = rhs.metallicTexture;
	roughnessTexture = rhs.roughnessTexture;
	aoTexture = rhs.aoTexture;
	emissionTexture = rhs.emissionTexture;

	textureID = TextureManager.GetTexture(albedoTexture);
	normalID = TextureManager.GetTexture(normalMap);
	metallicID = TextureManager.GetTexture(metallicTexture);
	roughnessID = TextureManager.GetTexture(roughnessTexture);
	ambientID = TextureManager.GetTexture(aoTexture);
	emissiveID = TextureManager.GetTexture(emissionTexture);

	return *this;
}

Material_instance& Material_instance::Duplicate_MaterialInstance(const Material_instance& other)
{
	// Copy each member variable from 'other' to 'this'

	shaderType = other.shaderType;
	name = other.name + " - Copy";
	albedoColour = other.albedoColour;
	metallicConstant = other.metallicConstant;
	roughnessConstant = other.roughnessConstant;
	aoConstant = other.aoConstant;
	emissionConstant = other.emissionConstant;

	albedoTexture = other.albedoTexture;
	normalMap = other.normalMap;
	metallicTexture = other.metallicTexture;
	roughnessTexture = other.roughnessTexture;
	aoTexture = other.aoTexture;
	emissionTexture = other.emissionTexture;

	Serialize(*this);

	return *this;
}