#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include "Core/SystemInterface.h"
#include "Scene/Entity.h"


#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemSingleThreaded.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"


ENGINE_RUNTIME_SYSTEM(PhysicsSystem)
{

	
	void Init();
	void Update();
	void Exit();

	void OnSceneStart();
	void OnSceneEnd();

	void CreatePhysicsObject(Entity& e);


	const unsigned int maxObjects = 1024;
	const unsigned int maxObjectMutexes = 0;
	const unsigned int maxBodyPairs = 1024;
	const unsigned int maxContactConstraints = 1024;

	unsigned int step = 0;

	//TODO:
	// Add BroadPhaseLayers
	// Add

	JPH::PhysicsSystem* physicsSystem = nullptr;
	JPH::BodyInterface* bodyInterface = nullptr;

};







#endif
