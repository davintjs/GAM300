/*!***************************************************************************************
\file			DemoSystem.cpp
\project
\author         Sean Ngo

\par			Course: GAM300
\date           18/09/2023

\brief
    This file contains the definitions of the following:
    1. Demo system for debugging and testing purposes

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "DemoSystem.h"
#include "IOManager/InputHandler.h"
#include "Scene/Scene.h"
#include "Core/Events.h"
#include "Core/EventsManager.h"
#include "Graphics/TextureManager.h"
#include "AssetManager/AssetManager.h"
#include "Scene/SceneManager.h"
#include "AI/NavMeshBuilder.h"

#define GET_TEXTURE_ID(filepath) TextureManager.GetTexture(AssetManager::Instance().GetAssetGUID(filepath));

void DemoSystem::Init()
{
	//CreateSceneEvent createScene(nullptr);
	//EVENTS.Publish(&createScene);
	//Scene& scene = *createScene.scene;
	MySceneManager.LoadScene("Assets/Scene/EuanTestingArena.scene");
	//Entity& testEntity = *scene.Add<Entity>();
	//MeshRenderer& entityRender = *scene.Add<MeshRenderer>(testEntity);
	//AudioSource& entityAudio = *scene.Add<AudioSource>(testEntity);
	//entityAudio.channel = AudioSource::Channel::MUSIC;
	////entityAudio.play = true;
	//entityAudio.currentSound = "LoLnvDie";
	//entityRender.MeshName = "DamagedHelmet";
	//entityRender.AlbedoTexture = "Default_albedo";
	//entityRender.textureID = GET_TEXTURE_ID(entityRender.AlbedoTexture);

	//entityRender.NormalMap = "Default_normal";
	//entityRender.normalMapID = GET_TEXTURE_ID(entityRender.NormalMap);

	//entityRender.RoughnessTexture = "Default_metalRoughness";
	//entityRender.RoughnessID = GET_TEXTURE_ID(entityRender.RoughnessTexture);

	//entityRender.MetallicTexture = "Default_metalRoughness";
	//entityRender.MetallicID = GET_TEXTURE_ID(entityRender.MetallicTexture);

	//entityRender.AoTexture = "Default_AO";
	//entityRender.AoID = GET_TEXTURE_ID(entityRender.AoTexture);

	//Entity& testEntity2 = *scene.Add<Entity>();
	//MeshRenderer& entityRender2 = *scene.Add<MeshRenderer>(testEntity2);
	//AudioSource& entityAudio2 = *scene.Add<AudioSource>(testEntity2);
	//entityAudio2.channel = AudioSource::Channel::SFX;
	////entityAudio2.play = true;
	//entityAudio2.currentSound = "Lan_YAY";
	//entityRender2.MeshName = "Skull_textured";
	//entityRender2.AlbedoTexture = "TD_Checker_Base_Color";
	//entityRender2.textureID = GET_TEXTURE_ID(entityRender2.AlbedoTexture);

	//entityRender2.NormalMap = "TD_Checker_Base_Color";
	//entityRender2.normalMapID = GET_TEXTURE_ID(entityRender2.NormalMap);

	//entityRender2.RoughnessTexture = "TD_Checker_Roughness";
	//entityRender2.RoughnessID = GET_TEXTURE_ID(entityRender2.RoughnessTexture);

	//entityRender2.AoTexture = "TD_Checker_Mixed_AO";
	//entityRender2.AoID = GET_TEXTURE_ID(entityRender2.AoTexture);

	//scene.Get<Transform>(testEntity).translation = Vector3(-150.f, 100.f, 0.f);
	//scene.Get<Transform>(testEntity).scale = Vector3(1.f, 1.f, 1.f);

	//// test instance rendering
	//for (int i = 0; i < 100; ++i)
	//{
	//	Entity& tempent = *scene.Add<Entity>();
	//	MeshRenderer& renderer = *scene.Add<MeshRenderer>(tempent);
	//	renderer.MeshName = "DamagedHelmet";
	//	renderer.AlbedoTexture = "Default_albedo";
	//	renderer.textureID = GET_TEXTURE_ID(renderer.AlbedoTexture);
	//	renderer.NormalMap = "Default_normal";
	//	renderer.normalMapID = GET_TEXTURE_ID(renderer.NormalMap);
	//	renderer.RoughnessTexture = "Default_metalRoughness";
	//	renderer.RoughnessID = GET_TEXTURE_ID(renderer.RoughnessTexture);
	//	renderer.MetallicTexture = "Default_metalRoughness";
	//	renderer.MetallicID = GET_TEXTURE_ID(renderer.MetallicTexture);
	//	renderer.AoTexture = "Default_AO";
	//	renderer.AoID = GET_TEXTURE_ID(renderer.AoTexture);
	//	scene.Get<Transform>(tempent).translation = Vector3((rand() % 5000) - 2500.f, (rand() % 5000) - 2500.f, (rand() % 5000) - 2500.f);
	//	scene.Add<MeshRenderer>(tempent);
	//}
	//scene.Add<Script>(scene.GetArray<Entity>()[0])->name = "Player";
	////scene.Add<Rigidbody>(scene.GetArray<Entity>()[0]);
	
	// Physics Tests
	// Entity& floor = *scene.Add<Entity>();
	// scene.Add<MeshRenderer>(floor);
	// scene.Get<MeshRenderer>(floor).MeshName = "Cube";
	// scene.Get<Transform>(floor).translation = Vector3(0.f, -10.f, 0.f);
	// scene.Get<Transform>(floor).scale = Vector3(300.f, 10.f, 300.f);
	// scene.Add<Rigidbody>(floor);
	// scene.Add<BoxCollider>(floor);

	//scene.Add<Script>(scene.GetArray<Entity>()[0],"Player");
	//scene.Add<Rigidbody>(scene.GetArray<Entity>()[0]);
	// Entity& box = *scene.Add<Entity>();
	// scene.Add<MeshRenderer>(box);
	// scene.Get<MeshRenderer>(box).MeshName = "Cube";
	// scene.Get<Transform>(box).translation = Vector3(0.f, 100.f, 0.f);
	// scene.Get<Transform>(box).scale = Vector3(25.f, 25.f, 25.f);
	// scene.Add<Rigidbody>(box);
	// scene.Add<BoxCollider>(box);

	// Entity& box2 = *scene.Add<Entity>();
	// scene.Add<MeshRenderer>(box2);
	// scene.Get<MeshRenderer>(box2).MeshName = "Cube";
	// scene.Get<Transform>(box2).translation = Vector3(0.f, 200.0f, 15.f);
	// scene.Get<Transform>(box2).scale = Vector3(25.f, 25.f, 25.f);
	// scene.Add<Rigidbody>(box2);
	// scene.Add<BoxCollider>(box2);


	// Entity& character = *scene.Add<Entity>();
	// scene.Add<MeshRenderer>(character);
	// scene.Get<MeshRenderer>(character).MeshName = "Cube";
	// scene.Get<Transform>(character).translation = Vector3(50.f, 200.0f, 0.f);
	// scene.Get<Transform>(character).scale = Vector3(25.f, 25.f, 25.f);

	//Entity& lightsource = *scene.Add<Entity>();
	//LightSource& lightsourcetemp = *scene.Add<LightSource>(lightsource);
	//lightsourcetemp.lightingColor = Vector3(100000.f, 100000.f, 100000.f);
}

void DemoSystem::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_B))
	{
		if (!NAVMESHBUILDER.GetNavMesh()) // First build
		{
			NAVMESHBUILDER.BuildNavMesh(); // Build the NavMesh
		}
		else // Rebaking
		{
			NAVMESHBUILDER.Exit(); // Clear current NavMesh
			NAVMESHBUILDER.BuildNavMesh(); // Rebuild NavMesh
		}
	}
}

void DemoSystem::Exit()
{

}