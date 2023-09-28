/*!***************************************************************************************
\file			DemoSystem.cpp
\project
\author         Sean Ngo

\par			Course: GAM300
\date           18/09/2023

\brief
    This file contains the definitions of the following:
    1.

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"

#include "DemoSystem.h"
#include "Scene/Scene.h"
#include "Core/Events.h"
#include "Core/EventsManager.h"

void DemoSystem::Init()
{
	CreateSceneEvent createScene(nullptr);
	EVENTS.Publish(&createScene);

	Scene& scene = *createScene.scene;
	//Entity& titty = *scene.Add<Entity>();
	//MeshRenderer& titrender = *scene.Add<MeshRenderer>(titty);
	//titrender.MeshName = "DamagedHelmet";
	//titrender.AlbedoTexture = "Default_albedo";
	//titrender.NormalMap = "Default_normal";
	//titrender.RoughnessTexture = "Default_metalRoughness";
	//titrender.MetallicTexture = "Default_metalRoughness";
	//titrender.AoTexture = "Default_AO";

	//scene.Get<Transform>(titty).translation = Vector3(150.f, 100.f, 0.f);
	//scene.Get<Transform>(titty).scale = Vector3(1.f, 1.f, 1.f);

	//Entity& titty2 = *scene.Add<Entity>();
	//MeshRenderer& titrender2 = *scene.Add<MeshRenderer>(titty2);
	//titrender2.MeshName = "Skull_textured";
	//titrender2.AlbedoTexture = "TD_Checker_Base_Color";
	//titrender2.NormalMap = "TD_Checker_Base_Color";
	//titrender2.RoughnessTexture = "TD_Checker_Roughness";
	//titrender2.AoTexture = "TD_Checker_Mixed_AO";
	//scene.Get<Transform>(titty).translation = Vector3(-150.f, 100.f, 0.f);
	//scene.Get<Transform>(titty).scale = Vector3(1.f, 1.f, 1.f);

	//// test instance rendering
	//for (int i = 0; i < 5; ++i)
	//{
	//	Entity& tempent = *scene.Add<Entity>();
	//	MeshRenderer& renderer = *scene.Add<MeshRenderer>(tempent);
	//	renderer.MeshName = "Cube";
	//	scene.Get<Transform>(tempent).translation = Vector3((rand() % 1000) - 500.f, (rand() % 1000) - 500.f, (rand() % 1000) - 500.f);
	//	scene.Get<Transform>(tempent).scale = Vector3((rand() % 50), (rand() % 50), (rand() % 50));
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
	////scene.Add<LightSource>(lightsource);
	//LightSource& lightsourcetemp = *scene.Add<LightSource>(lightsource);

	//lightsourcetemp.lightingColor = Vector3(150.f, 150.f, 150.f);
}

void DemoSystem::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);
}

void DemoSystem::Exit()
{

}