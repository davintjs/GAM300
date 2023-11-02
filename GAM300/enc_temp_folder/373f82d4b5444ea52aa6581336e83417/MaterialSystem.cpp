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
	
	Material_instance temp;

	_material[SHADERTYPE::PBR].push_back(temp);
	std::cout << "map size : " << _material.size();
	std::cout << "vector size : " << _material[SHADERTYPE::PBR].size();
}

void MaterialSystem::createPBR_NonInstanced()
{

	//Material_Types.push_back(SHADERTYPE::DEFAULT);


}
