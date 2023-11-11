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


void MaterialSystem::Init()
{
	EVENTS.Subscribe(this, &MaterialSystem::CallbackMaterialAssetLoaded);

	//NewMaterialInstance();

	// Creating PBR (Instanced) Material
	createPBR_Instanced();

	// Creating PBR (Non-Instanced) Material
	createPBR_NonInstanced();

	//temp 	
	available_shaders.push_back(Shader("Default", SHADERTYPE::DEFAULT));
	available_shaders.push_back(Shader("PBR", SHADERTYPE::PBR));
	
}

void MaterialSystem::Update(float dt)
{
	// Empty by design
}

void MaterialSystem::Exit()
{
	// Empty by design

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

Engine::GUID MaterialSystem::NewMaterialInstance(std::string _name)
{
	Material_instance defaultMaterial;
	defaultMaterial.name = _name;
	Engine::GUID guid = Engine::GUID();
	_allMaterialInstances[guid] = defaultMaterial;
	return guid;
}

void MaterialSystem::deleteInstance(Engine::GUID& matGUID)
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



void MaterialSystem::LoadMaterial(const MaterialAsset& _materialAsset, const Engine::GUID& _guid)
{

	//_allMaterialInstances[_guid](Deserialize(_materialAsset.mFilePath));
	Deserialize(_allMaterialInstances[_guid], _materialAsset.mFilePath);

}


void MaterialSystem::CallbackMaterialAssetLoaded(AssetLoadedEvent<MaterialAsset>* pEvent)
{

	LoadMaterial(pEvent->asset, pEvent->guid);

}


Material_instance& MaterialSystem::getMaterialInstance(Engine::GUID matGUID)
{

	static Material_instance defaultInstance;

	auto iter = _allMaterialInstances.find(matGUID);

	if (iter != _allMaterialInstances.end())
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

	albedoTexture = DEFAULT_TEXTURE;
	normalMap = DEFAULT_TEXTURE;
	metallicTexture = DEFAULT_TEXTURE;
	roughnessTexture = DEFAULT_TEXTURE;
	aoTexture = DEFAULT_TEXTURE;
	emissionTexture = DEFAULT_TEXTURE;
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