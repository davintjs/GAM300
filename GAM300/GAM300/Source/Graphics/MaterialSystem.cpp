#include "Precompiled.h"
#include "GraphicsHeaders.h"
#include "Scene/Components.h"
#include "Scene/SceneManager.h"


void MaterialSystem::Init()
{

	NewMaterialInstance();

	// Creating PBR (Instanced) Material
	createPBR_Instanced();

	// Creating PBR (Non-Instanced) Material
	createPBR_NonInstanced();


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
	Material_instance emissionMat;
	emissionMat.name = "Emission material";
	emissionMat.albedoColour = glm::vec4(0.f, 255.f, 255.f, 255.f);
	emissionMat.metallicConstant = 0.3f;
	emissionMat.roughnessConstant = 0.2f;
	emissionMat.aoConstant = 1.f;

	Material_instance blackSurfaceMat;
	blackSurfaceMat.name = "Black Surface material";
	blackSurfaceMat.albedoColour = glm::vec4(0.f, 0.f, 0.f, 255.f);
	blackSurfaceMat.metallicConstant = 0.2f;
	blackSurfaceMat.roughnessConstant = 1.f;
	blackSurfaceMat.aoConstant = 1.f;

	Material_instance darkBlueMat;
	darkBlueMat.name = "Dark Blue material";
	darkBlueMat.albedoColour = glm::vec4(0.f, 0.f, 255.f, 255.f);
	darkBlueMat.metallicConstant = 0.4f;
	darkBlueMat.roughnessConstant = 0.5f;
	darkBlueMat.aoConstant = 1.f;


	_material[SHADERTYPE::PBR].push_back(emissionMat);
	_material[SHADERTYPE::PBR].push_back(blackSurfaceMat);
	_material[SHADERTYPE::PBR].push_back(darkBlueMat);

	//deleteInstance(blackSurfaceMat);
}

void MaterialSystem::AddMaterial(const Material_instance& new_mat) {
	_material[new_mat.shaderType].push_back(new_mat);
}


void MaterialSystem::createPBR_NonInstanced()
{

	//Material_Types.push_back(SHADERTYPE::DEFAULT);


}

Material_instance& MaterialSystem::DuplicateMaterial(const Material_instance& instance)
{
	Material_instance new_mat = Material_instance(instance);
	new_mat.name += " - copy";
	_material[SHADERTYPE::PBR].push_back(new_mat);
	return _material[SHADERTYPE::PBR][_material[SHADERTYPE::PBR].size() - 1];
}


Material_instance& MaterialSystem::NewMaterialInstance(std::string _name)
{
	Material_instance defaultMaterial;
	defaultMaterial.name = _name;
	_material[defaultMaterial.shaderType].push_back(defaultMaterial);
	return *(_material[defaultMaterial.shaderType].end() - 1);
}

void MaterialSystem::deleteInstance(Material_instance& matInstance)
{
	Scene& currentScene = SceneManager::Instance().GetCurrentScene();

	// Swap them out to default material before deleting the instance
	for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
	{
		if (renderer.material_ptr == NULL)
		{
			continue;
		}

		if (renderer.material_ptr->name == matInstance.name)// change to matInstanceName
		{
			renderer.material_ptr = &(_material[matInstance.shaderType][0]);
		}
	}

	// Deleting the material instance
	for (std::vector<Material_instance>::iterator iter(_material[matInstance.shaderType].begin());
		iter != _material[matInstance.shaderType].end();
		++iter)
	{
		if ((*iter).name == matInstance.name)// change to matInstanceName
		{
			iter = _material[matInstance.shaderType].erase(iter);
		}


	}

}
