/*!***************************************************************************************
\file			PhysicsSystem.h
\project
\author         Matthew Lau
\co-authors		Desmond Too

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the declarations of:
	1. Physics System
	2. ObjectLayerPairFilter
	3. BroadPhaseLayerInterface
	4. ObjectvsBroadPhaseLayerFilter
	5. EngineContactListener

All content ? 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H
#include "Core/SystemInterface.h"
#include "Core/Events.h"

// Jolt includes
#include "Jolt/Jolt.h"
#include "Jolt/RegisterTypes.h"
#include "Jolt/Core/Factory.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/Shape/BoxShape.h"
#include "Jolt/Physics/Collision/Shape/SphereShape.h"
#include "Jolt/Physics/Collision/Shape/CapsuleShape.h"
#include "Jolt/Physics/Collision/Shape/ConvexShape.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Body/BodyActivationListener.h"
#include "Jolt/Physics/Character/Character.h"
#include "Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h"

// Layers that objects can be in and determines which other objects it can collide with
namespace EngineObjectLayers {
	inline constexpr JPH::ObjectLayer STATIC = 0;
	inline constexpr JPH::ObjectLayer DYNAMIC = 1;
	inline constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};
namespace EngineBroadPhaseLayers {
	inline constexpr JPH::BroadPhaseLayer STATIC(0);
	inline constexpr JPH::BroadPhaseLayer DYNAMIC(1);
	inline constexpr unsigned int NUM_LAYERS = 2;
};

 //Determines if two object layers should collide
class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer obj1, JPH::ObjectLayer obj2) const override {
		switch (obj1) {
		case EngineObjectLayers::STATIC:
			return obj2 == EngineObjectLayers::DYNAMIC;
		case EngineObjectLayers::DYNAMIC:
			return true;
		default:
			return false;

		}
	}

};

// Defines mapping between object and broadphase layers
class BroadPhaseLayerInterface final: public JPH::BroadPhaseLayerInterface {
public:
	BroadPhaseLayerInterface() {
		bpLayers[EngineObjectLayers::STATIC] = EngineBroadPhaseLayers::STATIC;
		bpLayers[EngineObjectLayers::DYNAMIC] = EngineBroadPhaseLayers::DYNAMIC;
	}
	virtual unsigned int GetNumBroadPhaseLayers() const override {
		return EngineBroadPhaseLayers::NUM_LAYERS;

	}
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer objLayer) const override {
		// TODO
		// Error checking!!

		return bpLayers[objLayer];
	}

	#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)EngineBroadPhaseLayers::STATIC:	return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)EngineBroadPhaseLayers::DYNAMIC:		return "MOVING";
		}
	}
	#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer bpLayers[EngineObjectLayers::NUM_LAYERS];
};

// Determines if an object layer can collide with a broadphase layer
class ObjectvsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
public:
	virtual bool ShouldCollide(JPH::ObjectLayer objLayer, JPH::BroadPhaseLayer bPLayer) const override {
		switch (objLayer) {
		case EngineObjectLayers::STATIC:
			return bPLayer == EngineBroadPhaseLayers::DYNAMIC;
		case EngineObjectLayers::DYNAMIC:
			return true;
		default:
			return false;
		}
	}
};


// Contact Listener
class EngineContactListener : public JPH::ContactListener {
public:
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body& body1, const JPH::Body& body2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& collisionResult) override;
	virtual void OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) override;
	virtual void OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) override;
	virtual void OnContactRemoved(const JPH::SubShapeIDPair& subShapePair) override;
};
#pragma region In Progress
//class EngineCollisionData {
//public:
//	EngineCollisionData() = default;
//
//private:
//	JPH::BodyID& body1;
//	JPH::BodyID& body2;
//
//	Vector3 body1CollisionPos;
//	Vector3 body2CollisionPos;
//
//};
#pragma endregion

ENGINE_RUNTIME_SYSTEM(PhysicsSystem)
{
public:
	void Init();
	void Update(float dt);
	void Exit();


	void PopulatePhysicsWorld();
	void UpdateGameObjects();
	void TestRun();


	// Scene Callbacks
	void CallbackSceneStart(SceneStartEvent* pEvent);
	void CallbackSceneStop(SceneStopEvent* pEvent);


	// Jolt Body creations
	//void CreateJoltRigidbody(Rigidbody & rb);
	//void CreateJoltCharacter(CharacterController & cc);

	const unsigned int maxObjects =					 1024;
	const unsigned int maxObjectMutexes =				0;
	const unsigned int maxObjectPairs =				 1024;
	const unsigned int maxContactConstraints =		 1024;


	unsigned int step = 0;

	JPH::TempAllocatorImpl* tempAllocator =		  nullptr;
	JPH::JobSystemThreadPool* jobSystem =		  nullptr;
	EngineContactListener* engineContactListener;

	JPH::PhysicsSystem* physicsSystem =			  nullptr;
	JPH::BodyInterface* bodyInterface =			  nullptr;

	JPH::BoxShapeSettings* floorShapeSettings =   nullptr;
	JPH::ShapeRefC* floorShape =				  nullptr;
	JPH::SphereShape* sphereShape =				  nullptr;
	JPH::CapsuleShape* capsuleShape =			  nullptr;


	BroadPhaseLayerInterface bpLayerInterface;
	ObjectLayerPairFilter objectLayerPairFilter;
	ObjectvsBroadPhaseLayerFilter objvbpLayerFilter;



};





#endif
