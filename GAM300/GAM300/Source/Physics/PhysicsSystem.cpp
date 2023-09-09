#include "Precompiled.h"
#include "PhysicsSystem.h"

void PhysicsSystem::Init() 
{
	// Register allocation hook
	JPH::RegisterDefaultAllocator();

	//// Create factory
	JPH::Factory::sInstance = new JPH::Factory();

	//// Register all JPH types
	JPH::RegisterTypes();

	//// Allocate memory for use in simulation
	tempAllocator.Allocate(10 * 1024 * 1024);
}
void PhysicsSystem::Update(float dt) {
	
	if (!simulating)
		return;

	// Update physics simulation by a step
	// TODO: change to use engine dt
	step++;
	physicsSystem->Update(dt, 1, &tempAllocator, &jobSystem);

	// Update gameobject positions with corresponding physics object positions
	/*
	* TODO:
	* 1. how to link a game object to the physics object
	*	- rigidbody component hold the bodyid?
	*	- rigidbody component hold body ptr?
	*/ 

	//UpdateGameObjects();

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


	// Create the JPH physics world and INIT it
	physicsSystem = new JPH::PhysicsSystem();

	// Get ptr to JPH Body interface which is used to access JPH bodies (physics objects)
	bodyInterface = &(physicsSystem->GetBodyInterface());

	// Initialize Physics World
	physicsSystem->Init(maxObjects, maxObjectMutexes, maxObjectPairs, maxContactConstraints,
		bpLayerInterface, objvbpLayerFilter, objectLayerPairFilter);

	// Optimise broad phase only if there is an excess amount of bodies
	//physicsSystem->OptimizeBroadPhase();

	simulating = true;

}
void PhysicsSystem::OnSceneEnd() {


	// Clear all existing Physics Objects
	if (physicsSystem) {
		delete physicsSystem;
		physicsSystem = nullptr;
	}
	simulating = false;

}
/*
void PhysicsSystem::PopulatePhysicsWorld() {
	Scene& scene = MySceneManager.GetCurrentScene();
	// check entity for collider and then check what kind of fucking collider he want
	// Shape Setting -> Shape Result -> Shape Refc -> Body Creation Setting -> Body
	auto& rbArray = scene.GetComponentsArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		Entity& entity = scene.GetEntity(rb);

		if (!scene.entities.IsActiveDense(entity.denseIndex))
			continue;
		if (!it.IsActive())
			continue;

		// If no collider is attached with the rigidbody, reject gameobject
		if (!scene.HasComponent<BoxCollider>(entity) || !scene.HasComponent<SphereCollider>(entity) || !scene.HasComponent<CapsuleCollider>(entity))
			continue;

		// Position, Rotation and Scale of collider
		Transform& t = scene.GetComponent<Transform>(entity);
		JPH::RVec3 scale;
		JPH::RVec3 pos;
		GlmVec3ToJoltVec3(t.translation, pos);
		JPH::RVec3 rot;
		GlmVec3ToJoltVec3(t.rotation, rot);

		// Linear + Angular Velocity
		JPH::RVec3 linearVel;
		GlmVec3ToJoltVec3(rb.linearVelocity, linearVel);
		JPH::RVec3 angularVel;
		GlmVec3ToJoltVec3(rb.angularVelocity, angularVel);

		// Set enabled status
		JPH::EActivation enabledStatus = JPH::EActivation::Activate;
		if (!rb.is_enabled)
			enabledStatus = JPH::EActivation::DontActivate;

		// Motion Type
		JPH::EMotionType motionType = JPH::EMotionType::Dynamic;
		if (rb.isStatic) {
			motionType = JPH::EMotionType::Static;
		}
		else if (rb.isKinematic) {
			motionType = JPH::EMotionType::Kinematic;
		}

		if (scene.HasComponent<BoxCollider>(entity)) {

			BoxCollider& bc = scene.GetComponent<BoxCollider>(entity);
			Vector3 colliderScale(bc.x * t.scale.x, bc.y * t.scale.y, bc.z * t.scale.z);
			GlmVec3ToJoltVec3(colliderScale, scale);


			JPH::BodyCreationSettings boxCreationSettings(new JPH::BoxShape(scale), pos, JPH::Quat::sIdentity(), motionType, EngineObjectLayers::DYNAMIC);
			if (rb.isStatic)
				boxCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;
			// Set all necessary settings for the body
			// Friction
			boxCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			boxCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			boxCreationSettings.mAngularVelocity = angularVel;

			JPH::Body* box = bodyInterface->CreateBody(boxCreationSettings);
			rb.RigidBodyID = box->GetID();

		}
		else if (scene.HasComponent<SphereCollider>(entity)) {

			SphereCollider& sc = scene.GetComponent<SphereCollider>(entity);
			JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(sc.radius), pos, JPH::Quat::sIdentity(), motionType, EngineObjectLayers::DYNAMIC);
			JPH::Body* sphere = bodyInterface->CreateBody(sphereCreationSettings);
			rb.RigidBodyID = sphere->GetID();

			if (rb.isStatic)
				sphereCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;

			// Set all necessary settings for the body
			// Friction
			sphereCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			sphereCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			sphereCreationSettings.mAngularVelocity = angularVel;

			bodyInterface->AddBody(sphere->GetID(),enabledStatus);

		}
		else if (scene.HasComponent<CapsuleCollider>(entity)) {


			CapsuleCollider& cc = scene.GetComponent<CapsuleCollider>(entity);
			JPH::BodyCreationSettings capsuleCreationSettings(new JPH::CapsuleShape(cc.height, cc.radius), pos, JPH::Quat::sIdentity(), motionType, EngineObjectLayers::DYNAMIC);
			JPH::Body* capsule = bodyInterface->CreateBody(capsuleCreationSettings);
			rb.RigidBodyID = capsule->GetID();

			if (rb.isStatic)
				capsuleCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;

			// Set all necessary settings for the body
			// Friction
			capsuleCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			capsuleCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			capsuleCreationSettings.mAngularVelocity = angularVel;

			bodyInterface->AddBody(capsule->GetID(), enabledStatus);
		}
		else {
			continue;
		}


		

	}

}

void PhysicsSystem::UpdateGameObjects() {
	JPH::BodyIDVector bidVector;
	physicsSystem->GetBodies(bidVector);

	Scene& scene = MySceneManager.GetCurrentScene();
	auto& rbArray = scene.GetComponentsArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		Entity& entity = scene.GetEntity(rb);
		Transform& t = scene.GetComponent<Transform>(entity);


		JPH::RVec3 tmp = bodyInterface->GetCenterOfMassPosition(rb.RigidBodyID);
		//convert
		//t.translation = tmp;
		// rinse and repeat for rotation and scale
		//update velocity

	}
}


void PhysicsSystem::TestRun() {

	// Create the JPH physics world and INIT it
	physicsSystem = new JPH::PhysicsSystem();

	// Get ptr to JPH Body interface which is used to access JPH bodies (physics objects)
	bodyInterface = &(physicsSystem->GetBodyInterface());

	// Initialize Physics World
	physicsSystem->Init(maxObjects, maxObjectMutexes, maxObjectPairs, maxContactConstraints,
		bpLayerInterface, objvbpLayerFilter, objectLayerPairFilter);

	// Optimise broad phase only if there is an excess amount of bodies
	//physicsSystem->OptimizeBroadPhase();

	//Creating a rigid body that will be used as a floor 
	//For this, we create the settings for the collision volume such as the shape 
	floorShapeSettings = new JPH::BoxShapeSettings(JPH::Vec3(100.0f, 1.0f, 100.0f));

	//Creating the shape 
	JPH::ShapeSettings::ShapeResult floorShapeResult = floorShapeSettings->Create();

	floorShape = new JPH::ShapeRefC(floorShapeResult.Get()); //	Can also check for HasError() or GetError() 

	//Creating the settings for the body itself 
	JPH::BodyCreationSettings floorSettings(*floorShape, JPH::RVec3(0.0, -1.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, EngineObjectLayers::STATIC);

	JPH::Body* floor = bodyInterface->CreateBody(floorSettings);

	//Add it to the real world 
	bodyInterface->AddBody(floor->GetID(), JPH::EActivation::DontActivate);

	//Next, we add a dynamic body (ball) to test 
	sphereShape = new JPH::SphereShape(0.5f);
	JPH::BodyCreationSettings sphereSettings(sphereShape, JPH::RVec3(0.0, 2.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
	JPH::BodyID sphere_ID = bodyInterface->CreateAndAddBody(sphereSettings, JPH::EActivation::Activate);

	//To give the body a velocity as we will be interacting with it 
	bodyInterface->SetLinearVelocity(sphere_ID, JPH::Vec3(0.0f, -5.0f, 0.0f));

	simulating = true;

	//Creating a capsule shape 
	capsuleShape = new JPH::CapsuleShape(1.0f, 0.5f);
	JPH::BodyCreationSettings capsuleSettings(capsuleShape, JPH::RVec3(0.0, 2.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
	JPH::BodyID capsule_ID = bodyInterface->CreateAndAddBody(capsuleSettings, JPH::EActivation::Activate);

	bodyInterface->SetLinearVelocity(capsule_ID, JPH::Vec3(0.0f, -5.0f, 0.0f));


	//
	//PopulatePhysicsWorld();
}
*/
// Math conversion helpers
void GlmVec3ToJoltVec3(Vector3& gVec3, JPH::RVec3& jVec3) {
	jVec3.SetX(gVec3.x);
	jVec3.SetY(gVec3.y);
	jVec3.SetZ(gVec3.z);
}
void GlmVec3ToJoltQuat(Vector3& gVec3, JPH::Quat& jQuat) {
	JPH::RVec3 tmp;
	GlmVec3ToJoltVec3(gVec3, tmp);

	jQuat = JPH::Quat::sEulerAngles(tmp);

}
void JoltVec3ToGlmVec3(JPH::RVec3& jVec3, Vector3& gVec3) {
	gVec3.x = jVec3.GetX();
	gVec3.y = jVec3.GetY();
	gVec3.z = jVec3.GetZ();
}
void JoltQuatToGlmVec3(JPH::Quat& jQuat, Vector3& gVec3) {
	JPH::RVec3 tmp = jQuat.GetEulerAngles();
	JoltVec3ToGlmVec3(tmp, gVec3);
}
