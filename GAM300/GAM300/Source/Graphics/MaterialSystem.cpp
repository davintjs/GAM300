#include "Precompiled.h"
#include "GraphicsHeaders.h"
#include "Scene/Components.h"


void MaterialSystem::Init()
{
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
	
	//Material_Types.push_back(SHADERTYPE::PBR);
	//_material.insert(SHADERTYPE::PBR)

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
}

void MaterialSystem::createPBR_NonInstanced()
{

	//Material_Types.push_back(SHADERTYPE::DEFAULT);


}
