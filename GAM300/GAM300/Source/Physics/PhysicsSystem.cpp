#include "Precompiled.h"
#include "PhysicsSystem.h"

void PhysicsSystem::Init() 
{
	// Register allocation hook
	JPH::RegisterDefaultAllocator();

	// Create factory
	JPH::Factory::sInstance = new JPH::Factory();

	// Register all JPH types
	JPH::RegisterTypes();

	// Allocate memory for use in simulation
	tempAllocator.Allocate(10 * 1024 * 1024);

}

//void PhysicsSystem::Update() {
//	
//	if (!simulating)
//		return;
//
//	// Update physics simulation by a step
//	// TODO: change to use engine dt
//	step++;
//	physicsSystem->Update(1.0f / 60.0f, 1, &tempAllocator, &jobSystem);
//
//	// Update gameobject positions with corresponding physics object positions
//	/*
//	* TODO:
//	* 1. how to link a game object to the physics object
//	*	- rigidbody component hold the bodyid?
//	*	- rigidbody component hold body ptr?
//	*/ 
//
//
//	JPH::BodyIDVector bidVector;
//	physicsSystem->GetBodies(bidVector);
//	for (JPH::BodyID& bid : bidVector) {
//		// update accordingly
//		JPH::RVec3 pos = bodyInterface->GetCenterOfMassPosition(bid);
//	}
//
//}
//
//void PhysicsSystem::Exit() {
//
//	// Unregister JPH types and cleans up default material
//	JPH::UnregisterTypes();
//
//	// Destroy Factory
//	if (JPH::Factory::sInstance){
//		delete JPH::Factory::sInstance;
//		JPH::Factory::sInstance = nullptr;
//	}
//
//	// Destroy Physics World
//	if (physicsSystem) {
//		delete physicsSystem;
//		physicsSystem = nullptr;
//	}
//
//}
//
//void PhysicsSystem::OnSceneStart() {
//
//
//	// Create the JPH physics world and INIT it
//	physicsSystem = new JPH::PhysicsSystem();
//
//	// Get ptr to JPH Body interface which is used to access JPH bodies (physics objects)
//	bodyInterface = &(physicsSystem->GetBodyInterface());
//
//	// Initialize Physics World
//	physicsSystem->Init(maxObjects, maxObjectMutexes, maxObjectPairs, maxContactConstraints,
//		bpLayerInterface, objvbpLayerFilter, objectLayerPairFilter);
//
//	// Optimise broad phase only if there is an excess amount of bodies
//	//physicsSystem->OptimizeBroadPhase();
//	
//	
//	// Iterate through rigidbody list
//	// for each rigidbody component, create a "Jolt Body" using the entity the rb component belongs to
//	
//
//
//	//Creating a rigid body that will be used as a floor 
//	//For this, we create the settings for the collision volume such as the shape 
//	floorShapeSettings = new JPH::BoxShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f));
//
//	//Creating the shape 
//	JPH::ShapeSettings::ShapeResult floorShapeResult = floorShapeSettings->Create();
//
//	floorShape = new JPH::ShapeRefC(floorShapeResult.Get()); //	Can also check for HasError() or GetError() 
//
//	//Creating the settings for the body itself 
//	JPH::BodyCreationSettings floorSettings(*floorShape, JPH::RVec3(0.0, -1.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, EngineObjectLayers::STATIC);
//
//	JPH::Body* floor = bodyInterface->CreateBody(floorSettings);
//
//	//Add it to the real world 
//	bodyInterface->AddBody(floor->GetID(), JPH::EActivation::DontActivate);
//
//	//Next, we add a dynamic body (ball) to test 
//	sphereShape = new JPH::SphereShape(0.5f);
//
//	JPH::BodyCreationSettings sphereSettings(sphereShape, JPH::RVec3(0.0, 2.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
//	JPH::BodyID sphere_ID = bodyInterface->CreateAndAddBody(sphereSettings, JPH::EActivation::Activate);
//
//	//To give the body a velocity as we will be interacting with it 
//	bodyInterface->SetLinearVelocity(sphere_ID, JPH::Vec3(0.0f, -5.0f, 0.0f)); 
//
//	simulating = true;
//
//}
//void PhysicsSystem::OnSceneEnd() {
//
//
//	// Clear all existing Physics Objects
//	if (physicsSystem) {
//		delete physicsSystem;
//		physicsSystem = nullptr;
//	}
//	simulating = false;
//
//}
//
//void PhysicsSystem::CreatePhysicsObject(Entity& e) {
//
//	// check entity for collider and then check what kind of fucking collider he want
//	// Shape Setting -> Shape Result -> Shape Refc -> Body Creation Setting -> Body
//	
//
//
//
//
//}
