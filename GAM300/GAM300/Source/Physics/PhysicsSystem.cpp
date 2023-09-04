#include "PhysicsSystem.h"

void PhysicsSystem::Init() {

	// Register allocation hook
	JPH::RegisterDefaultAllocator();

	// Create factory
	JPH::Factory::sInstance = new JPH::Factory();

	// Register all JPH types
	JPH::RegisterTypes();

	// Create the JPH physics world and INIT it
	physicsSystem = new JPH::PhysicsSystem();

	// Get ptr to JPH Body interface which is used to access JPH bodies (physics objects)
	bodyInterface = &(physicsSystem->GetBodyInterface());


}

void PhysicsSystem::Update() {
	step++;

	// Simulate physics

	// Update gameobject positions with corresponding physics object positions


}

void PhysicsSystem::Exit() {

	// Unregister JPH types and cleans up default material
	JPH::UnregisterTypes();

	// Destroy Factory
	if (JPH::Factory::sInstance){
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}

	// Destroy Physics World
	if (physicsSystem) {



		delete physicsSystem;
		physicsSystem = nullptr;
	}

}

void PhysicsSystem::OnSceneStart() {


}
void PhysicsSystem::OnSceneEnd() {


}

void PhysicsSystem::CreatePhysicsObject(Entity& e) {


}
