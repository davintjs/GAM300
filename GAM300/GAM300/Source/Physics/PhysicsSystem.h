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
	inline constexpr JPH::ObjectLayer SENSOR = 2;
	inline constexpr JPH::ObjectLayer NUM_LAYERS = 3;
};
// For broadphase layers
namespace EngineBroadPhaseLayers {
	inline constexpr JPH::BroadPhaseLayer STATIC(0);
	inline constexpr JPH::BroadPhaseLayer DYNAMIC(1);
	inline constexpr JPH::BroadPhaseLayer SENSOR(2);

	inline constexpr unsigned int NUM_LAYERS = 3;
};

 //Determines if two object layers should collide
class ObjectLayerPairFilter : public JPH::ObjectLayerPairFilter {
public:
	// Check if two objects can collide or not, depending on the object layer they are in
	virtual bool ShouldCollide(JPH::ObjectLayer obj1, JPH::ObjectLayer obj2) const override {
		switch (obj1) {
		case EngineObjectLayers::STATIC:
			return obj2 == EngineObjectLayers::DYNAMIC;
		case EngineObjectLayers::DYNAMIC:
			return true;
		case EngineObjectLayers::SENSOR:
			return obj2 == EngineObjectLayers::DYNAMIC || EngineObjectLayers::SENSOR;
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
		bpLayers[EngineObjectLayers::SENSOR] = EngineBroadPhaseLayers::SENSOR;

	}
	// Get number of broadphase layers
	virtual unsigned int GetNumBroadPhaseLayers() const override {
		return EngineBroadPhaseLayers::NUM_LAYERS;

	}
	// Get a broadphase layer
	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer objLayer) const override {
		// TODO
		// Error checking!!

		return bpLayers[objLayer];
	}

	#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	// Get the name of a broadphase layer
	virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
		case (JPH::BroadPhaseLayer::Type)EngineBroadPhaseLayers::STATIC:	return "NON_MOVING";
		case (JPH::BroadPhaseLayer::Type)EngineBroadPhaseLayers::DYNAMIC:		return "MOVING";
		default:
			return "NON_MOVING";
		}
	}
	#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

private:
	JPH::BroadPhaseLayer bpLayers[EngineObjectLayers::NUM_LAYERS];
};

// Determines if an object layer can collide with a broadphase layer
class ObjectvsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter {
public:

	// Check if an object in an object layer should collide with a broadphase layer
	virtual bool ShouldCollide(JPH::ObjectLayer objLayer, JPH::BroadPhaseLayer bPLayer) const override {
		switch (objLayer) {
		case EngineObjectLayers::STATIC:
			return bPLayer == EngineBroadPhaseLayers::DYNAMIC;
		case EngineObjectLayers::DYNAMIC:
			return true;
		case EngineObjectLayers::SENSOR:
			return bPLayer == EngineBroadPhaseLayers::DYNAMIC;
		default:
			return false;
		}
	}
};

// Holds data that will be used to resolve any collisions filtered so that collision callbacks to the game objects involved are triggered
struct EngineCollisionData {
public:

	enum collisionOperation : char {
		added = 0,
		persisted,
		removed
	};

	EngineCollisionData(EngineCollisionData::collisionOperation cop) : op{ cop } {}

	UINT32 bid1;
	UINT32 bid2;


	collisionOperation op;

};

// Contact Listener (collision)
class EngineContactListener : public JPH::ContactListener {
public:
	EngineContactListener() : pSystem{ nullptr } {}
	// Callback to validate a collision (contact)
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body& body1, const JPH::Body& body2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& collisionResult) override;
	// Callback when new collision is registered
	virtual void OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) override;
	// Callback when a collision persists to next update frame
	virtual void OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) override;
	// Callback for when a collision is removed
	virtual void OnContactRemoved(const JPH::SubShapeIDPair& subShapePair) override;

	JPH::PhysicsSystem* pSystem;
	std::vector<EngineCollisionData> collisionResolution;
};

#pragma region Testing

class CharacterControllerTest {
public:
	JPH::Ref<JPH::Character> mCharacter = nullptr;
	~CharacterControllerTest() {
		if(mCharacter)
			mCharacter->RemoveFromPhysicsSystem();
	}
};

#pragma endregion

ENGINE_RUNTIME_SYSTEM(PhysicsSystem)
{
public:

	// Initialise the Physics System
	void Init();
	// Updates the Physics System
	void Update(float dt);
	// Clean-up duty
	void Exit();

	// Clone any gameobject with rigidbodies, character controller into a Jolt Body for simulations
	void PopulatePhysicsWorld();

	// Update the transform and other data of gameobjects with new values after simulating the physics
	void UpdateGameObjects();

	// Resolve any updates before Physics Simulation
	void PrePhysicsUpdate(float dt);
	// Resolve any updates after Physics Simulation but before next frame
	void PostPhysicsUpdate();

	// Resolve any character controller movement
	void ResolveCharacterMovement();

	// A testing function
	void TestRun();

	// Callback function for when scene preview starts
	void CallbackSceneStart(SceneStartEvent* pEvent);
	// Callback function for when scene preview stops
	void CallbackSceneStop(SceneStopEvent* pEvent);

	void DeleteBody(PhysicsComponent& bid);
	void DeleteBody(UINT32 bid);

	const unsigned int maxObjects =						1024;
	const unsigned int maxObjectMutexes =				   0;
	const unsigned int maxObjectPairs =					1024;
	const unsigned int maxContactConstraints =			1024;

	float characterCollisionTolerance =					0.05f;

	unsigned int step = 0;
	float accumulatedTime = 0.f;

	JPH::TempAllocatorImpl* tempAllocator =			nullptr;
	JPH::JobSystemThreadPool* jobSystem =			nullptr;
	EngineContactListener* engineContactListener =	nullptr;

	JPH::PhysicsSystem* physicsSystem =				nullptr;
	JPH::BodyInterface* bodyInterface =				nullptr;

	JPH::BoxShapeSettings* floorShapeSettings =		nullptr;
	JPH::ShapeRefC* floorShape =					nullptr;
	JPH::SphereShape* sphereShape =					nullptr;
	JPH::CapsuleShape* capsuleShape =				nullptr;


	BroadPhaseLayerInterface bpLayerInterface;
	ObjectLayerPairFilter objectLayerPairFilter;
	ObjectvsBroadPhaseLayerFilter objvbpLayerFilter;


	#pragma region Character Controller Testing
	float mTime = 0.f;
	CharacterControllerTest* ccTest =				nullptr;
	std::vector<JPH::Ref<JPH::Character>> characters;
	#pragma endregion

};



#endif
