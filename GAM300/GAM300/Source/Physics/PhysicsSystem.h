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

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

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
class BroadPhaseLayerInterface : public JPH::BroadPhaseLayerInterface {
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
	const unsigned int maxObjectPairs = 1024;
	const unsigned int maxContactConstraints = 1024;

	unsigned int step = 0;

	bool simulating = false;

	JPH::PhysicsSystem* physicsSystem = nullptr;
	JPH::BodyInterface* bodyInterface = nullptr;

	JPH::BoxShapeSettings* floorShapeSettings = nullptr;
	JPH::ShapeRefC* floorShape = nullptr;
	JPH::BodyCreationSettings* floorSettings = nullptr;
	JPH::SphereShape* sphereShape = nullptr;

	//BroadPhaseLayerInterface bpLayerInterface;
	//ObjectLayerPairFilter objectLayerPairFilter;
	//ObjectvsBroadPhaseLayerFilter objvbpLayerFilter;

	JPH::TempAllocatorImpl tempAllocator;
	JPH::JobSystemSingleThreaded jobSystem;
};






#endif
