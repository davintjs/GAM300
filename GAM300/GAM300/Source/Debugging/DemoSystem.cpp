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
	Entity& titty = *scene.Add<Entity>();
	MeshRenderer& titrender = *scene.Add<MeshRenderer>(titty);
	titrender.MeshName = "DamagedHelmet";
	titrender.AlbedoTexture = "Default_albedo";
	titrender.NormalMap = "Default_normal";
	titrender.RoughnessTexture = "Default_metalRoughness";
	titrender.MetallicTexture = "Default_metalRoughness";
	titrender.AoTexture = "Default_AO";

	scene.Get<Transform>(titty).translation = Vector3(150.f, 100.f, 0.f);
	scene.Get<Transform>(titty).scale = Vector3(1.f, 1.f, 1.f);

	Entity& titty2 = *scene.Add<Entity>();
	MeshRenderer& titrender2 = *scene.Add<MeshRenderer>(titty2);
	titrender2.MeshName = "Skull_textured";
	titrender2.AlbedoTexture = "TD_Checker_Base_Color";
	titrender2.NormalMap = "TD_Checker_Base_Color";
	titrender2.RoughnessTexture = "TD_Checker_Roughness";
	titrender2.AoTexture = "TD_Checker_Mixed_AO";
	scene.Get<Transform>(titty).translation = Vector3(-150.f, 100.f, 0.f);
	scene.Get<Transform>(titty).scale = Vector3(1.f, 1.f, 1.f);

	scene.Add<Script>(scene.GetArray<Entity>()[0],"Player");
	scene.Add<Rigidbody>(scene.GetArray<Entity>()[0]);
	Entity& box = *scene.Add<Entity>();
	scene.Add<MeshRenderer>(box);
	scene.Get<Transform>(box).translation = Vector3(0.f, 100.f, 0.f);
	scene.Get<Transform>(box).scale = Vector3(25.f, 25.f, 25.f);

	Entity& box2 = *scene.Add<Entity>();
	scene.Add<MeshRenderer>(box2);
	scene.Get<Transform>(box2).translation = Vector3(0.f, 200.0f, 35.f);
	scene.Get<Transform>(box2).scale = Vector3(25.f, 25.f, 25.f);

	Entity& lightsource = *scene.Add<Entity>();
	//scene.Add<LightSource>(lightsource);
	LightSource& lightsourcetemp = *scene.Add<LightSource>(lightsource);

	lightsourcetemp.lightingColor = Vector3(150.f, 150.f, 150.f);
}

void DemoSystem::Update(float dt)
{

}

void DemoSystem::Exit()
{

}