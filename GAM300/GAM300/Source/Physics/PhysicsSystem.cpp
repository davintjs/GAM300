/*!***************************************************************************************
\file			PhysicsSystem.cpp
\project
\author         Matthew Lau
\co-authors		Desmond Too

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the definitions of:
	1. Physics System
	2. Math conversion helper functions

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/
#include "Precompiled.h"
#include "Core/EventsManager.h"
#include "PhysicsSystem.h"
#include "Scene/SceneManager.h"
#include "Core/FramerateController.h"
#include "IOManager/InputHandler.h"

JPH::BodyID testBallID;
JPH::BodyID testBallID2;

Entity* ball = nullptr;
Entity* ball2 = nullptr;

// Convert glm::Vec3 to JPH::Vec3
void GlmVec3ToJoltVec3(Vector3& gVec3, JPH::RVec3& jVec3);
// Convert glm::Vec3 to JPH::Quat
void GlmVec3ToJoltQuat(Vector3& gVec3, JPH::Quat& jQuat);
// Convert JPH::Vec3 to glm::Vec3 
void JoltVec3ToGlmVec3(JPH::RVec3& jVec3, Vector3& gVec3);
// Convert JPH::Quat to glm::Vec3 
void JoltQuatToGlmVec3(JPH::Quat& jQuat, Vector3& gVec3);

// Create a Jolt character
void CreateJoltCharacter(CharacterController& cc, JPH::PhysicsSystem* psystem, PhysicsSystem* enginePSystem);

void PhysicsSystem::Init() 
{
	EVENTS.Subscribe(this, &PhysicsSystem::CallbackSceneStart);
	EVENTS.Subscribe(this, &PhysicsSystem::CallbackSceneStop);

	// Register allocation hook
	JPH::RegisterDefaultAllocator();

	// Create factory
	JPH::Factory::sInstance = new JPH::Factory();

	// Register all JPH types
	JPH::RegisterTypes();

	tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
	
	jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency()-1);
	
	engineContactListener = new EngineContactListener();
}
void PhysicsSystem::Update(float dt) {
	// Handle Inputs
	if (!physicsSystem)
		return;

	Scene& scene = MySceneManager.GetCurrentScene();
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);
		

		Transform& t = scene.Get<Transform>(entity);

		if (scene.Has<BoxCollider>(entity))
		{
			BoxCollider& test = scene.Get<BoxCollider>(entity);

		}

		Vector3 tmpVec;
		JPH::BodyID tmpBID(rb.bid);
		JPH::RVec3 tmp;
		Vector3 translation = t.GetTranslation();
		GlmVec3ToJoltVec3(translation, tmp);
		bodyInterface->SetPosition(tmpBID, tmp, JPH::EActivation::Activate);

		JPH::Quat tmpQuat;
		Vector3 rotation = t.GetRotation();
		GlmVec3ToJoltQuat(rotation, tmpQuat);
		bodyInterface->SetRotation(tmpBID, tmpQuat,JPH::EActivation::Activate);


		GlmVec3ToJoltVec3(rb.linearVelocity, tmp);
		bodyInterface->SetLinearVelocity(tmpBID, tmp);
		//std::cout << "velocity:" << rb.linearVelocity.x << ',' << rb.linearVelocity.y << ',' << rb.linearVelocity.z << std::endl;
		GlmVec3ToJoltVec3(rb.angularVelocity, tmp);
		bodyInterface->SetAngularVelocity(tmpBID, tmp);

	}


	auto& ccArray = scene.GetArray<CharacterController>();
	int j = 0;
	for (auto it = ccArray.begin(); it != ccArray.end(); ++it)
	{
		CharacterController& cc = *it;
		JPH::BodyID tmpBid{ cc.bid };

		Transform& t = scene.Get<Transform>(cc);
		//JPH::RVec3 scale;
		JPH::RVec3 pos;
		Vector3 tpos = t.GetTranslation();
		GlmVec3ToJoltVec3(tpos, pos);
		JPH::Quat rot;
		Vector3 trot = t.GetRotation();
		GlmVec3ToJoltQuat(trot, rot);
		//bodyInterface->SetPosition(tmpBid, pos, JPH::EActivation::Activate);
		//bodyInterface->SetRotation(tmpBid, rot, JPH::EActivation::Activate);
		if (characters[j]->GetGroundState() == JPH::Character::EGroundState::OnGround)
		{
			//std::cout << "Character " << i << " is grounded\n";
			cc.isGrounded = true;
		}
		else
		{
			cc.isGrounded = false;
		}
		++j;
	}

	//std::cout << "pre update\n";

	// Fixed time steps
	if (physicsSystem) {

		float fixedDt = (float)MyFrameRateController.GetFixedDt();
		for (int i = 0; i < MyFrameRateController.GetSteps(); ++i)
		{
			
			PrePhysicsUpdate(dt);
			physicsSystem->Update(fixedDt, 1, tempAllocator, jobSystem);

		}
	//	accumulatedTime = 0.f;
	}
//	else {
	//	accumulatedTime += dt;
	//}
	//std::cout << "after physics update but before post update\n";
	//std::cout << "DT: " << dt << std::endl;
	//std::cout << "Physics update!\n";	

	PostPhysicsUpdate();

}
void PhysicsSystem::Exit() {

	// Unregister JPH types and cleans up default material
	JPH::UnregisterTypes();

	// Destroy Factory
	if (JPH::Factory::sInstance){
		delete JPH::Factory::sInstance;
		JPH::Factory::sInstance = nullptr;
	}


	delete engineContactListener;

	PRINT("CLEANING UP PHYSICS\n");
	// Clean up any characters
	for (JPH::Ref<JPH::Character> r : characters) {

		if (r == ccTest->mCharacter)
			continue;

		r->RemoveFromPhysicsSystem();

	}
	characters.clear();

	// Delete the current physics system, must set to nullptr
	if (ccTest) {
		//ccTest->mCharacter->RemoveFromPhysicsSystem();
		delete ccTest;
		ccTest = nullptr;
	}

	// Destroy Physics World
	if (physicsSystem) {
		delete physicsSystem;
		physicsSystem = nullptr;
	}

	if (jobSystem) {
		delete jobSystem;
		jobSystem = nullptr;
	}

	if (tempAllocator) {
		delete tempAllocator;
		tempAllocator = nullptr;
	}

}

void PhysicsSystem::PrePhysicsUpdate(float dt) {
	/*
	// Input handling for character
	JPH::Vec3 direction = JPH::Vec3::sZero();
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_W) || InputHandler::isKeyButtonHolding(GLFW_KEY_W)) {
		std::cout << "move forward\n";
		direction.SetX(10);
	}
	else if (InputHandler::isKeyButtonPressed(GLFW_KEY_A) || InputHandler::isKeyButtonHolding(GLFW_KEY_A)) {
		direction.SetZ(-10);
	}
	else if (InputHandler::isKeyButtonPressed(GLFW_KEY_D) || InputHandler::isKeyButtonHolding(GLFW_KEY_D)) {
		direction.SetZ(10);
	}
	else if (InputHandler::isKeyButtonPressed(GLFW_KEY_S) || InputHandler::isKeyButtonHolding(GLFW_KEY_S)) {
		direction.SetX(-10);
	}
	if (direction != JPH::Vec3::sZero())
		direction = direction.Normalized();

	bool jump = false;
	if (InputHandler::isKeyButtonPressed(GLFW_KEY_SPACE))
		jump = true;


	JPH::Character::EGroundState groundState = ccTest->mCharacter->GetGroundState();
	if (groundState == JPH::Character::EGroundState::OnSteepGround
		|| groundState == JPH::Character::EGroundState::NotSupported)
	{
		JPH::Vec3 normal = ccTest->mCharacter->GetGroundNormal();
		normal.SetY(0.0f);
		float dot = normal.Dot(direction);
		if (dot < 0.0f)
			direction -= (dot * normal) / normal.LengthSq();
	}
	direction.SetY(0);
	
	// Update velocity
	if (ccTest->mCharacter->IsSupported()) {
		JPH::Vec3 current_velocity = ccTest->mCharacter->GetLinearVelocity();
		JPH::Vec3 desired_velocity = 10.f * direction;
		desired_velocity.SetY(current_velocity.GetY());
		JPH::Vec3 new_velocity = 0.75f * current_velocity + 0.25f * desired_velocity;

		// Jump
		if (jump && groundState == JPH::Character::EGroundState::OnGround)
			new_velocity += JPH::Vec3(0, 40.f, 0);

		ccTest->mCharacter->SetLinearVelocity(new_velocity);
	}
	*/

	// Resolve any character controller moves
	ResolveCharacterMovement();

}
void PhysicsSystem::PostPhysicsUpdate() {
	//std::cout << "Post physics update\n";
	//std::cout << engineContactListener->collisionResolution.size() << std::endl;
	for (EngineCollisionData& e : engineContactListener->collisionResolution) {
		// Find rigidbody components of the two bodies
		Rigidbody* rb1 = nullptr;
		Rigidbody* rb2 = nullptr;
		bool found = false;
		Scene& scene = MySceneManager.GetCurrentScene();
		auto& rbArray = scene.GetArray<Rigidbody>();
		for (auto it = rbArray.begin(); it != rbArray.end() && !found; ++it) {

			Rigidbody& rb = *it;
			if (rb.bid == e.bid1) {
				rb1 = &rb;
			}
			else if (rb.bid == e.bid2) {
				rb2 = &rb;
			}

			if (rb1 && rb2)
				found = true;
		}

		if (!rb1 || !rb2)
			continue;

		// Publish the right event
		if (e.op == EngineCollisionData::collisionOperation::added) {

			// Trigger or Collision
			if (rb1->is_trigger || rb2->is_trigger) {
				TriggerEnterEvent tee;
				tee.rb1 = rb1;
				tee.rb2 = rb2;
				EVENTS.Publish(&tee);
				//std::cout << "Trigger Enter!\n";
			}
			else {
				ContactAddedEvent cae;
				cae.rb1 = rb1;
				cae.rb2 = rb2;
				EVENTS.Publish(&cae);
				//std::cout << "Collision Enter!\n";
			}
		}
		else if (e.op == EngineCollisionData::collisionOperation::removed) {

			// Trigger or Collision
			if (rb1->is_trigger || rb2->is_trigger) {
				TriggerRemoveEvent tre;
				tre.rb1 = rb1;
				tre.rb2 = rb2;
				EVENTS.Publish(&tre);
				//std::cout << "Trigger Remove!\n";
			}
			else {
				ContactRemovedEvent cre;
				cre.rb1 = rb1;
				cre.rb2 = rb2;
				EVENTS.Publish(&cre);
				//std::cout << "Collision Remove!\n";
			}
		}

	}
	engineContactListener->collisionResolution.clear();

	//if(ccTest->mCharacter != nullptr)
	//	ccTest->mCharacter->PostSimulation(0.05f);
	for (auto it = characters.begin(); it != characters.end(); ++it) {
		(*it)->PostSimulation(0.05f);
	}

	UpdateGameObjects();
}

void PhysicsSystem::ResolveCharacterMovement() {


	//std::cout << "Resolve Character Movement\n";
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& ccArray = scene.GetArray<CharacterController>();

	for (auto it = ccArray.begin(); it != ccArray.end(); ++it) {
		CharacterController& cc = *it;
		JPH::Ref<JPH::Character> mCharacter = nullptr;
		for (JPH::Ref<JPH::Character> r : characters) {
			if (cc.bid == r->GetBodyID().GetIndexAndSequenceNumber()) {
				mCharacter = r;
				break;
			}
		}

		if (mCharacter == nullptr)
			continue;

		JPH::Vec3 direction;
		GlmVec3ToJoltVec3(cc.direction, direction);
		//if (cc.direction != Vector3(0, 0, 0)) {
		//	std::cout << "direction:" << cc.direction.x << ',' << cc.direction.y << ',' << cc.direction.z << std::endl;
		//}

		JPH::Character::EGroundState groundState = mCharacter->GetGroundState();
		if (groundState == JPH::Character::EGroundState::OnSteepGround
			|| groundState == JPH::Character::EGroundState::NotSupported)
		{
			JPH::Vec3 normal = mCharacter->GetGroundNormal();
			normal.SetY(0.0f);
			float dot = normal.Dot(direction);
			if (dot < 0.0f)
				direction -= (dot * normal) / normal.LengthSq();
		}


		// Update velocity
		if (mCharacter->IsSupported()) {
			JPH::Vec3 current_velocity = mCharacter->GetLinearVelocity();
			JPH::Vec3 desired_velocity = direction;
			desired_velocity.SetY(current_velocity.GetY());
			JPH::Vec3 new_velocity = 0.75f * current_velocity + 0.25f * desired_velocity;

			// Jump
			if (groundState != JPH::Character::EGroundState::OnGround) {
				new_velocity.SetY(0);
			}
			else {

				new_velocity += JPH::Vec3(0, direction.GetY(), 0);
			}


			mCharacter->SetLinearVelocity(new_velocity);
		}

		cc.direction = Vector3(0, 0, 0);
	}

}

void PhysicsSystem::CallbackSceneStart(SceneStartEvent* pEvent) 
{
	UNREFERENCED_PARAMETER(pEvent);
	// Create the JPH physics world and INIT it
	physicsSystem = new JPH::PhysicsSystem();
	ccTest = new CharacterControllerTest();

	// Get ptr to JPH Body interface which is used to access JPH bodies (physics objects)
	bodyInterface = &(physicsSystem->GetBodyInterface());

	// Initialize Physics World
	physicsSystem->Init(maxObjects, maxObjectMutexes, maxObjectPairs, maxContactConstraints,
		bpLayerInterface, objvbpLayerFilter, objectLayerPairFilter);

	engineContactListener->pSystem = physicsSystem;

	physicsSystem->SetContactListener(engineContactListener);
	// Optimise broad phase only if there is an excess amount of bodies
	//physicsSystem->OptimizeBroadPhase();

	PopulatePhysicsWorld();


	std::cout << "Physics System scene start test\n";

}



void PhysicsSystem::CallbackSceneStop(SceneStopEvent* pEvent) 
{
	UNREFERENCED_PARAMETER(pEvent);
	std::cout << "Physics System scene stop test\n";

	// Clean up any characters
	for (JPH::Ref<JPH::Character> r : characters) {

		if (r == ccTest->mCharacter)
			continue;

		r->RemoveFromPhysicsSystem();

	}
	characters.clear();

	// Delete the current physics system, must set to nullptr
	if (physicsSystem) {
		//ccTest->mCharacter->RemoveFromPhysicsSystem();
		delete ccTest;
		ccTest = nullptr;
		delete physicsSystem;
		physicsSystem = nullptr;

	}

	engineContactListener->pSystem = nullptr;
}

void PhysicsSystem::PopulatePhysicsWorld() {
	Scene& scene = MySceneManager.GetCurrentScene();

	// check entity for collider and then check what kind of fucking collider he want
	// Shape Setting -> Shape Result -> Shape Refc -> Body Creation Setting -> Body
	auto& rbArray = scene.GetArray<Rigidbody>();
	//auto& bcArray = scene.GetArray<BoxCollider>();
	std::cout << "Number of rigidbodies:" << rbArray.size() << std::endl;
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
			
	
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);
		

		if (!scene.IsActive(entity))
			continue;
		if (!it.IsActive())
			continue;



		// If no collider is attached with the rigidbody, reject gameobject
		if (!scene.Has<BoxCollider>(entity) && !scene.Has<SphereCollider>(entity) && !scene.Has<CapsuleCollider>(entity))
			continue;

		// Position, Rotation and Scale of collider
		Transform& t = scene.Get<Transform>(entity);
		JPH::RVec3 scale;
		JPH::RVec3 pos;
		Vector3 tpos = t.GetTranslation();
		GlmVec3ToJoltVec3(tpos, pos);
		JPH::Quat rot;
		Vector3 trot = t.GetRotation();
		GlmVec3ToJoltQuat(trot, rot);

		// Linear + Angular Velocity
		JPH::RVec3 linearVel;
		GlmVec3ToJoltVec3(rb.linearVelocity, linearVel);
		JPH::RVec3 angularVel;
		GlmVec3ToJoltVec3(rb.angularVelocity, angularVel);

		// Set enabled status
		JPH::EActivation enabledStatus = JPH::EActivation::Activate;
		//if (!rb.a)
		//	enabledStatus = JPH::EActivation::DontActivate;

		// Motion Type
		JPH::EMotionType motionType = JPH::EMotionType::Dynamic;
		if (rb.isStatic) {
			motionType = JPH::EMotionType::Static;
		}
		else if (rb.isKinematic) {
			motionType = JPH::EMotionType::Kinematic;
		}

		
		// Create rigidbody's collider shape
		if (scene.Has<BoxCollider>(entity)) {

			BoxCollider& boxCollider = scene.Get<BoxCollider>(entity);
			Vector3 colliderScale(boxCollider.x * t.scale.x/2.f, boxCollider.y * t.scale.y/2.f, boxCollider.z * t.scale.z/2.f);
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
			// Sensor settings 
			boxCreationSettings.mIsSensor = rb.is_trigger;
			boxCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			boxCreationSettings.mMassPropertiesOverride.mMass = 1.0f;
			// Create the actual jolt body
			JPH::Body* box = bodyInterface->CreateBody(boxCreationSettings);
			bodyInterface->AddBody(box->GetID(), enabledStatus);
			rb.bid = box->GetID().GetIndexAndSequenceNumber();

		}
		else if (scene.Has<SphereCollider>(entity)) {

			SphereCollider& sc = scene.Get<SphereCollider>(entity);
			JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(sc.radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			if (rb.isStatic)
				sphereCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;

			// Set all necessary settings for the body
			// Friction
			sphereCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			sphereCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			sphereCreationSettings.mAngularVelocity = angularVel;
			// Sensor settings
			sphereCreationSettings.mIsSensor = rb.is_trigger;

			JPH::Body* sphere = bodyInterface->CreateBody(sphereCreationSettings);
			bodyInterface->AddBody(sphere->GetID(),enabledStatus);
			rb.bid = sphere->GetID().GetIndexAndSequenceNumber();
		}
		else if (scene.Has<CapsuleCollider>(entity)) {


			CapsuleCollider& cc = scene.Get<CapsuleCollider>(entity);
			JPH::BodyCreationSettings capsuleCreationSettings(new JPH::CapsuleShape(cc.height, cc.radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);

			if (rb.isStatic)
				capsuleCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;

			// Set all necessary settings for the body
			// Friction
			capsuleCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			capsuleCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			capsuleCreationSettings.mAngularVelocity = angularVel;
			// Sensor settings
			capsuleCreationSettings.mIsSensor = rb.is_trigger;

			JPH::Body* capsule = bodyInterface->CreateBody(capsuleCreationSettings);
			bodyInterface->AddBody(capsule->GetID(), enabledStatus);
			rb.bid = capsule->GetID().GetIndexAndSequenceNumber();
		}
		else {
			continue;
		}


	}


	// Character Controller
	// Note: ideally there should only be 1
	auto& ccArray = scene.GetArray<CharacterController>();
	for (auto it = ccArray.begin(); it != ccArray.end(); ++it) {
		CreateJoltCharacter(*it, physicsSystem, this);
	}




	std::cout << "Number of jolt bodies:" << physicsSystem->GetNumBodies() << std::endl;


}

void PhysicsSystem::UpdateGameObjects() {

	if (!physicsSystem)
		return;

	Scene& scene = MySceneManager.GetCurrentScene();
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);
		Transform& t = scene.Get<Transform>(entity);


		Vector3 tmpVec;
		JPH::BodyID tmpBID(rb.bid);
		JPH::RVec3 tmp = bodyInterface->GetCenterOfMassPosition(tmpBID);
		JoltVec3ToGlmVec3(tmp, tmpVec);	
		t.translation = tmpVec;


		JPH::Quat tmpQuat = bodyInterface->GetRotation(tmpBID);
		JoltQuatToGlmVec3(tmpQuat, tmpVec);
		t.rotation = tmpVec;

		tmp = bodyInterface->GetLinearVelocity(tmpBID);
		JoltVec3ToGlmVec3(tmp, rb.linearVelocity);

		tmp = bodyInterface->GetAngularVelocity(tmpBID);
		JoltVec3ToGlmVec3(tmp, rb.angularVelocity);


		//Transform& t = scene.Get<Transform>(*ball);
		//t.translation = gBallPos;


		//Vector3 ballRotEuler;
		//JPH::Quat ballQuat = bodyInterface->GetRotation(testBallID);
		//JoltQuatToGlmVec3(ballQuat, ballRotEuler);
		//t.rotation = ballRotEuler;

	}

	auto& ccArray = scene.GetArray<CharacterController>();
	for (auto it = ccArray.begin(); it != ccArray.end(); ++it) {
		CharacterController& cc = *it;
		Entity& entity = scene.Get<Entity>(cc);
		Transform& t = scene.Get<Transform>(entity);

		JPH::BodyID tmpBID(cc.bid);
		JPH::RVec3 tmp = ccTest->mCharacter->GetLinearVelocity();
		JoltVec3ToGlmVec3(tmp, cc.velocity);

		tmp = ccTest->mCharacter->GetCenterOfMassPosition();
		JoltVec3ToGlmVec3(tmp, t.translation);

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

	physicsSystem->SetContactListener(engineContactListener);


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

	//PopulatePhysicsWorld();
}

// Create and add a Jolt Character to a Jolt physics system using a character controller
void CreateJoltCharacter(CharacterController& cc, JPH::PhysicsSystem* psystem, PhysicsSystem* enginePSystem) {

	Scene& scene = MySceneManager.GetCurrentScene();
	Entity& entity = scene.Get<Entity>(cc);
	Transform& t = scene.Get<Transform>(entity);

	JPH::Ref<JPH::CharacterSettings> characterSetting = new JPH::CharacterSettings;
	characterSetting->mMass = cc.mass;
	characterSetting->mFriction = cc.friction;
	characterSetting->mGravityFactor = cc.gravityFactor;
	characterSetting->mLayer = EngineObjectLayers::DYNAMIC;
	characterSetting->mMaxSlopeAngle = (cc.slopeLimit / 180.f) * 3.14f;	// converting to radian first
	characterSetting->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -t.scale.x);

	// Character Shape (default capsule)
	JPH::RefConst<JPH::Shape> characterShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, 0.5f*t.scale.y + t.scale.x, 0),
													JPH::Quat::sIdentity(), new JPH::CapsuleShape(0.5f * t.scale.y, t.scale.x)).Create().Get();
	characterSetting->mShape = characterShape;


	JPH::RVec3 pos;
	GlmVec3ToJoltVec3(t.translation, pos);
	JPH::Quat rot;
	GlmVec3ToJoltQuat(t.rotation, rot);

	JPH::Ref<JPH::Character> character = new JPH::Character(characterSetting, pos, rot, 0, psystem);
	JPH::EActivation activeStatus = JPH::EActivation::Activate;
	if (!scene.IsActive(cc))
		activeStatus = JPH::EActivation::DontActivate;

	character->AddToPhysicsSystem(activeStatus);
	cc.bid = character->GetBodyID().GetIndexAndSequenceNumber();
	
	// Main character = 1st character controller in scene
	if(!enginePSystem->ccTest->mCharacter)
		enginePSystem->ccTest->mCharacter = character;

	enginePSystem->characters.push_back(character);

	return;
}



#pragma region EngineContactListener
// Contact Listeners
JPH::ValidateResult EngineContactListener::OnContactValidate(const JPH::Body& body1, const JPH::Body& body2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult& collisionResult) {
	
	(void)body1;
	(void)body2;
	(void)inBaseOffset;
	(void)collisionResult;
	
	//std::cout << "Contact validate callback!\n";
	return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
}

void EngineContactListener::OnContactAdded(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) {
	(void)manifold;
	(void)ioSettings;
	if (!pSystem)
		return;
	// If bodies already have contact, do not register another OnCollisionEnter/OnTriggerEnter
	if (pSystem->WereBodiesInContact(body1.GetID(), body2.GetID()))
		return;

	collisionResolution.emplace_back(EngineCollisionData(EngineCollisionData::collisionOperation::added));
	collisionResolution.back().bid1 = body1.GetID().GetIndexAndSequenceNumber();
	collisionResolution.back().bid2 = body2.GetID().GetIndexAndSequenceNumber();


	//std::cout << "Contact Added\n";
}

void EngineContactListener::OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) 
{
	(void)body1;
	(void)body2;
	(void)manifold;
	(void)ioSettings;
	//std::cout << "Contact persisting!\n";
}

void EngineContactListener::OnContactRemoved(const JPH::SubShapeIDPair& subShapePair) {
	if (!pSystem)
		return;

	// If the bodies are still touching, do not register OnCollisionExit/OnTriggerExit
	if (!pSystem->WereBodiesInContact(subShapePair.GetBody1ID(), subShapePair.GetBody2ID())) {

	}
		//std::cout << "Contact Removed\n";

	collisionResolution.emplace_back(EngineCollisionData(EngineCollisionData::collisionOperation::removed));
	collisionResolution.back().bid1 = subShapePair.GetBody1ID().GetIndexAndSequenceNumber();
	collisionResolution.back().bid2 = subShapePair.GetBody2ID().GetIndexAndSequenceNumber();
}
#pragma endregion

// Math conversion helpers
void GlmVec3ToJoltVec3(Vector3& gVec3, JPH::RVec3& jVec3) 
{
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
