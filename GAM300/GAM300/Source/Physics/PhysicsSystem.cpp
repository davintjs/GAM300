#include "Precompiled.h"

#include "PhysicsSystem.h"
#include "Scene/SceneManager.h"


JPH::BodyID testBallID;
JPH::BodyID testBallID2;

Entity* ball = nullptr;
Entity* ball2 = nullptr;


void GlmVec3ToJoltVec3(Vector3& gVec3, JPH::RVec3& jVec3);
void GlmVec3ToJoltQuat(Vector3& gVec3, JPH::Quat& jQuat);
void JoltVec3ToGlmVec3(JPH::RVec3& jVec3, Vector3& gVec3);
void JoltQuatToGlmVec3(JPH::Quat& jQuat, Vector3& gVec3);

void PhysicsSystem::Init() 
{
	// Register allocation hook
	JPH::RegisterDefaultAllocator();

	//// Create factory
	JPH::Factory::sInstance = new JPH::Factory();

	//// Register all JPH types
	JPH::RegisterTypes();

	tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
	jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency()-1);
	
	
	
	
	TestRun();


}
void PhysicsSystem::Update(float dt) {

	if (!simulating)
		return;

	// Update physics simulation by a step
	// TODO: change to use engine dt
	step++;
	physicsSystem->Update(dt, 1, tempAllocator, jobSystem);

	// Update gameobject positions with corresponding physics object positions
	/*
	* TODO:
	* 1. how to link a game object to the physics object
	*	- rigidbody component hold the bodyid?
	*	- rigidbody component hold body ptr?
	*/ 

	//std::cout << "Physics System Update!\n";

	JPH::RVec3 ballPos = bodyInterface->GetCenterOfMassPosition(testBallID);
	Vector3 gBallPos;
	JoltVec3ToGlmVec3(ballPos, gBallPos);
	//std::cout << gBallPos.x << ',' << gBallPos.y << ',' << gBallPos.z << std::endl;

	Scene& scene = MySceneManager.GetCurrentScene();
	if (ball) 
	{
		Transform& t = scene.Get<Transform>(*ball);
		t.translation = gBallPos;


		Vector3 ballRotEuler;
		JPH::Quat ballQuat = bodyInterface->GetRotation(testBallID);
		JoltQuatToGlmVec3(ballQuat, ballRotEuler);
		t.rotation = ballRotEuler;

	}

	JPH::RVec3 ballPos2 = bodyInterface->GetCenterOfMassPosition(testBallID2);
	Vector3 gBallPos2;
	JoltVec3ToGlmVec3(ballPos2, gBallPos2);
	if (ball2) 
	{
		Transform& t = scene.Get<Transform>(*ball2);
		t.translation = gBallPos2;



		Vector3 ballRotEuler;
		JPH::Quat ballQuat = bodyInterface->GetRotation(testBallID2);
		JoltQuatToGlmVec3(ballQuat, ballRotEuler);
		t.rotation = ballRotEuler;
	}
		

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
	//physicsSystem->Init(maxObjects, maxObjectMutexes, maxObjectPairs, maxContactConstraints,
	//	bpLayerInterface, objvbpLayerFilter, objectLayerPairFilter);

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

void PhysicsSystem::PopulatePhysicsWorld() {
	Scene& scene = MySceneManager.GetCurrentScene();
	// check entity for collider and then check what kind of fucking collider he want
	// Shape Setting -> Shape Result -> Shape Refc -> Body Creation Setting -> Body
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);

		if (!scene.IsActive(entity))
			continue;
		if (!it.IsActive())
			continue;

		// If no collider is attached with the rigidbody, reject gameobject
		if (!scene.HasComponent<BoxCollider>(entity) || !scene.HasComponent<SphereCollider>(entity) || !scene.HasComponent<CapsuleCollider>(entity))
			continue;

		// Position, Rotation and Scale of collider
		Transform& t = scene.Get<Transform>(entity);
		JPH::RVec3 scale;
		JPH::RVec3 pos;
		GlmVec3ToJoltVec3(t.translation, pos);
		JPH::Quat rot;
		GlmVec3ToJoltQuat(t.rotation, rot);

		// Linear + Angular Velocity
		JPH::RVec3 linearVel;
		GlmVec3ToJoltVec3(rb.linearVelocity, linearVel);
		JPH::RVec3 angularVel;
		GlmVec3ToJoltVec3(rb.angularVelocity, angularVel);

		// Set enabled status
		JPH::EActivation enabledStatus = JPH::EActivation::Activate;
		//if (!rb.is_enabled)
		//	enabledStatus = JPH::EActivation::DontActivate;

		// Motion Type
		JPH::EMotionType motionType = JPH::EMotionType::Dynamic;
		if (rb.isStatic) {
			motionType = JPH::EMotionType::Static;
		}
		else if (rb.isKinematic) {
			motionType = JPH::EMotionType::Kinematic;
		}

		

		if (scene.HasComponent<BoxCollider>(entity)) {

			BoxCollider& bc = scene.Get<BoxCollider>(entity);
			Vector3 colliderScale(bc.x * t.scale.x, bc.y * t.scale.y, bc.z * t.scale.z);
			GlmVec3ToJoltVec3(colliderScale, scale);


			JPH::BodyCreationSettings boxCreationSettings(new JPH::BoxShape(scale), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			if (rb.isStatic)
				boxCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;
			// Set all necessary settings for the body
			// Friction
			boxCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			boxCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			boxCreationSettings.mAngularVelocity = angularVel;

			//Sensor settings 
			boxCreationSettings.mIsSensor = true;

			JPH::Body* box = bodyInterface->CreateBody(boxCreationSettings);
			//rb.RigidBodyID = box->GetID();

		}
		else if (scene.HasComponent<SphereCollider>(entity)) {

			SphereCollider& sc = scene.Get<SphereCollider>(entity);
			JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(sc.radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			JPH::Body* sphere = bodyInterface->CreateBody(sphereCreationSettings);
			//rb.RigidBodyID = sphere->GetID();

			if (rb.isStatic)
				sphereCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;

			// Set all necessary settings for the body
			// Friction
			sphereCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			//sphereCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			//sphereCreationSettings.mAngularVelocity = angularVel;

			bodyInterface->AddBody(sphere->GetID(),enabledStatus);

		}
		else if (scene.HasComponent<CapsuleCollider>(entity)) {


			CapsuleCollider& cc = scene.Get<CapsuleCollider>(entity);
			JPH::BodyCreationSettings capsuleCreationSettings(new JPH::CapsuleShape(cc.height, cc.radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			JPH::Body* capsule = bodyInterface->CreateBody(capsuleCreationSettings);
			//rb.RigidBodyID = capsule->GetID();

			if (rb.isStatic)
				capsuleCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;

			// Set all necessary settings for the body
			// Friction
			capsuleCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			//capsuleCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			//capsuleCreationSettings.mAngularVelocity = angularVel;

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
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);
		Transform& t = scene.Get<Transform>(entity);


		//JPH::RVec3 tmp = bodyInterface->GetCenterOfMassPosition(rb.RigidBodyID);
		//JoltVec3ToGlmVec3(tmp, t.translation);
		//convert
		//t.translation = tmp;
		// rinse and repeat for rotation and scale
		//update velocity

	}
}


void PhysicsSystem::TestRun() {

	ball = &MySceneManager.GetCurrentScene().GetArray<Entity>()[1];
	ball2 = &MySceneManager.GetCurrentScene().GetArray<Entity>()[2];

	// Create the JPH physics world and INIT it
	physicsSystem = new JPH::PhysicsSystem();

	// Get ptr to JPH Body interface which is used to access JPH bodies (physics objects)
	bodyInterface = &(physicsSystem->GetBodyInterface());

	// Initialize Physics World
	physicsSystem->Init(maxObjects, maxObjectMutexes, maxObjectPairs, maxContactConstraints,
		bpLayerInterface, objvbpLayerFilter, objectLayerPairFilter);

	physicsSystem->SetContactListener(&engineContactListener);


	// Optimise broad phase only if there is an excess amount of bodies
	//physicsSystem->OptimizeBroadPhase();
	std::cout << "Number of bodies before:" << physicsSystem->GetNumBodies() << std::endl;

	//Creating a rigid body that will be used as a floor 
	//For this, we create the settings for the collision volume such as the shape 
	floorShapeSettings = new JPH::BoxShapeSettings(JPH::Vec3(300.0f, 10.0f, 300.0f));
	//Creating the shape 
	JPH::ShapeSettings::ShapeResult floorShapeResult = floorShapeSettings->Create();
	floorShape = new JPH::ShapeRefC(floorShapeResult.Get()); //	Can also check for HasError() or GetError() 
	//Creating the settings for the body itself 
	JPH::BodyCreationSettings floorSettings(*floorShape, JPH::RVec3(0.0, 0.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, EngineObjectLayers::STATIC);
	JPH::Body* floor = bodyInterface->CreateBody(floorSettings);
	//Add it to the real world 
	bodyInterface->AddBody(floor->GetID(), JPH::EActivation::Activate);


	//Creating a rigid body that will be used as a floor 
	//For this, we create the settings for the collision volume such as the shape 
	JPH::BoxShapeSettings* bss = new JPH::BoxShapeSettings(JPH::Vec3(25.0f, 25.0f, 25.0f));
	//Creating the shape 
	JPH::ShapeSettings::ShapeResult boxShapeResult = bss->Create();
	JPH::ShapeRefC* boxShape = new JPH::ShapeRefC(boxShapeResult.Get()); //	Can also check for HasError() or GetError() 
	//Creating the settings for the body itself 
	JPH::BodyCreationSettings boxSettings(*boxShape, JPH::RVec3(0.0, 100.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
	JPH::Body* box = bodyInterface->CreateBody(boxSettings);
	//Add it to the real world 
	bodyInterface->AddBody(box->GetID(), JPH::EActivation::Activate);
	testBallID = box->GetID();

	//Creating a rigid body that will be used as a floor 
	//For this, we create the settings for the collision volume such as the shape 
	JPH::BoxShapeSettings* bss2 = new JPH::BoxShapeSettings(JPH::Vec3(25.0f, 25.0f, 25.0f));
	//Creating the shape 
	JPH::ShapeSettings::ShapeResult boxShapeResult2 = bss2->Create();
	JPH::ShapeRefC* boxShape2 = new JPH::ShapeRefC(boxShapeResult2.Get()); //	Can also check for HasError() or GetError() 
	//Creating the settings for the body itself 
	JPH::BodyCreationSettings boxSettings2(*boxShape2, JPH::RVec3(0.0, 200.0, 35.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
	JPH::Body* box2 = bodyInterface->CreateBody(boxSettings2);
	//Add it to the real world 
	bodyInterface->AddBody(box2->GetID(), JPH::EActivation::Activate);
	testBallID2 = box2->GetID();


	////Next, we add a dynamic body (ball) to test 
	//sphereShape = new JPH::SphereShape(25.0f);
	//JPH::BodyCreationSettings sphereSettings(sphereShape, JPH::RVec3(0.0, 100.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
	//JPH::BodyID sphere_ID = bodyInterface->CreateAndAddBody(sphereSettings, JPH::EActivation::Activate);
	////To give the body a velocity as we will be interacting with it 
	//bodyInterface->SetLinearVelocity(sphere_ID, JPH::Vec3(0.0f, 0.0f, 0.0f));
	//testBallID = sphere_ID;

	std::cout << "Number of bodies after:" << physicsSystem->GetNumBodies() << std::endl;


	simulating = true;

	////Creating a capsule shape 
	//capsuleShape = new JPH::CapsuleShape(1.0f, 0.5f);
	//JPH::BodyCreationSettings capsuleSettings(capsuleShape, JPH::RVec3(0.0, 2.0, 0.0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, EngineObjectLayers::DYNAMIC);
	//JPH::BodyID capsule_ID = bodyInterface->CreateAndAddBody(capsuleSettings, JPH::EActivation::Activate);

	//bodyInterface->SetLinearVelocity(capsule_ID, JPH::Vec3(0.0f, -5.0f, 0.0f));


	//
	//PopulatePhysicsWorld();
}

void CreateJoltCharacter(CharacterController& cc, JPH::PhysicsSystem* psystem) {

	JPH::CharacterSettings characterSetting;
	characterSetting.mMass = cc.mass;
	characterSetting.mFriction = cc.friction;
	characterSetting.mGravityFactor = cc.gravityFactor;
	characterSetting.mLayer = EngineObjectLayers::DYNAMIC;
	characterSetting.mMaxSlopeAngle = (cc.slopeLimit / 180.f) * 3.14f;	// converting to radian first




	return;
}

// Contact Listener
JPH::ValidateResult EngineContactListener::OnContactValidate(const JPH::Body& body1, const JPH::Body& body2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& collisionResult) {
	std::cout << "Contact validate callback!\n";
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}
void EngineContactListener::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) {
	std::cout << "Contact was added!\n";
}
void EngineContactListener::OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) {
	std::cout << "Contact persisting!\n";
}
void EngineContactListener::OnContactRemoved(const JPH::SubShapeIDPair& subShapePair) {
	std::cout << "Contact removed!\n";
}

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
