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
#include "Utilities/ThreadPool.h"

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
	// Event Subscriptions
	EVENTS.Subscribe(this, &PhysicsSystem::CallbackSceneStop);
	EVENTS.Subscribe(this, &PhysicsSystem::CallbackObjectCreated);
	EVENTS.Subscribe(this, &PhysicsSystem::CallbackObjectDestroyed);

	// Register allocation hook
	JPH::RegisterDefaultAllocator();

	// Create factory
	JPH::Factory::sInstance = new JPH::Factory();

	// Register all JPH types
	JPH::RegisterTypes();

	// Allocate chunk of memory for usage by Physics System
	tempAllocator = new JPH::TempAllocatorImpl(10 * 1024 * 1024);
	
	// Jolt uses multi-threading
	jobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, JPH::thread::hardware_concurrency()-1);
	
	// Basic contact listener
	engineContactListener = new EngineContactListener();
}

void PhysicsSystem::PostSubscription() 
{
	EVENTS.Subscribe(this, &PhysicsSystem::CallbackSceneStart);
}

void PhysicsSystem::Update(float dt) {

	if (!physicsSystem)
		return;

	// Update positions, linear velocity of bodies so that any scripting logic is applied for the physics simulation
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& rbArray = scene.GetArray<Rigidbody>();

	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;

		if (rb.state == DELETED) continue;

		Entity& entity = scene.Get<Entity>(rb);
		
		Vector3 tmpVec;
		JPH::BodyID tmpBID(rb.bid);
		JPH::RVec3 tmp;

		Transform& t = scene.Get<Transform>(entity);
		
		Vector3 translation = t.GetTranslation();
		if (scene.Has<BoxCollider>(entity))
			translation = translation.operator glm::vec3() + scene.Get<BoxCollider>(entity).offset.operator glm::vec3();
		GlmVec3ToJoltVec3(translation, tmp);
		bodyInterface->SetPosition(tmpBID, tmp, JPH::EActivation::DontActivate);

		JPH::Quat tmpQuat;
		Vector3 rotation = t.GetRotation();
		GlmVec3ToJoltQuat(rotation, tmpQuat);
		bodyInterface->SetRotation(tmpBID, tmpQuat,JPH::EActivation::DontActivate);


		GlmVec3ToJoltVec3(rb.linearVelocity, tmp);
		bodyInterface->SetLinearVelocity(tmpBID, tmp);
		GlmVec3ToJoltVec3(rb.angularVelocity, tmp);
		bodyInterface->SetAngularVelocity(tmpBID, tmp);
		GlmVec3ToJoltVec3(rb.force, tmp);
		bodyInterface->AddForce(tmpBID, tmp);
		rb.force = vec3(0);

		// Update awake/sleep state of bodies depending
		if (scene.IsActive(rb) && scene.IsActive(scene.Get<Entity>(rb)))
		{
			JPH::BodyLockWrite lock(physicsSystem->GetBodyLockInterface(), tmpBID);
			if (lock.Succeeded())
			{
				const JPH::Body& body = lock.GetBody();

				if (!body.IsActive() && !body.IsStatic()) {
					lock.ReleaseLock();
					bodyInterface->ActivateBody(tmpBID);

				}

			}

		}
		else {
			JPH::BodyLockWrite lock(physicsSystem->GetBodyLockInterface(), tmpBID);
			if (lock.Succeeded())
			{
				const JPH::Body& body = lock.GetBody();

				if (body.IsActive() && !body.IsStatic()) {
					//PRINT("deactivating a body\n");
					lock.ReleaseLock();
					bodyInterface->DeactivateBody(tmpBID);
					//std::cout << "num active bodies: " << physicsSystem->GetNumActiveBodies(JPH::EBodyType::RigidBody) << '\n';

				}

			}

		}
	}
	//std::cout << "num active bodies after rigidbodies are updated: " << physicsSystem->GetNumActiveBodies(JPH::EBodyType::RigidBody) << '\n';

	auto& ccArray = scene.GetArray<CharacterController>();
	int j = 0;
	for (auto it = ccArray.begin(); it != ccArray.end(); ++it) {
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

		JPH::RVec3 velocity;
		GlmVec3ToJoltVec3(cc.velocity, velocity);

		bodyInterface->SetPosition(tmpBid, pos, JPH::EActivation::DontActivate);
		bodyInterface->SetRotation(tmpBid, rot, JPH::EActivation::DontActivate);
		bodyInterface->SetLinearVelocity(tmpBid, velocity);
		//if (characters[j]->GetGroundState() == JPH::Character::EGroundState::OnGround)
		//{
		//	//std::cout << "Character " << i << " is grounded\n";
		//	cc.isGrounded = true;
		//}
		//else
		//{
		//	cc.isGrounded = false;
		//}
		++j;
	}
	
	// Fixed time steps
	if (physicsSystem) {

		float fixedDt = (float)MyFrameRateController.GetFixedDt();
		for (int i = 0; i < MyFrameRateController.GetSteps(); ++i)
		{

			PrePhysicsUpdate(dt);
			physicsSystem->Update(fixedDt, 1, tempAllocator, jobSystem);
			step++;
			//std::cout << "num active bodies after update: " << physicsSystem->GetNumActiveBodies(JPH::EBodyType::RigidBody) << '\n';

		}
	}
	
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

	if (engineContactListener) {
		delete engineContactListener;
		engineContactListener = nullptr;
	}

	//PRINT("CLEANING UP PHYSICS\n");
	// Clean up any characters
	for (JPH::Ref<JPH::Character> r : characters) {

		if (r == ccTest->mCharacter)
			continue;

		r->RemoveFromPhysicsSystem();

	}
	characters.clear();

	// Delete the current physics system, must set to nullptr
	if (ccTest) {
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

	// Resolve any character controller movement from scripting system
	ResolveCharacterMovement();

}
void PhysicsSystem::PostPhysicsUpdate() {
	ACQUIRE_SCOPED_LOCK(PhysicsCollision);
	//std::cout << "Post physics update\n";

	// Handle collision events
	for (EngineCollisionData& e : engineContactListener->collisionResolution) {
		// Find rigidbody components of the two bodies
		PhysicsComponent* pc1 = nullptr;
		PhysicsComponent* pc2 = nullptr;
		bool found = false;
		Scene& scene = MySceneManager.GetCurrentScene();
		auto& rbArray = scene.GetArray<Rigidbody>();
		for (auto it = rbArray.begin(); it != rbArray.end() && !found; ++it) 
		{

			Rigidbody& rb = *it;

			if (rb.state == DELETED) continue;

			if (rb.bid == e.bid1) {
				pc1 = &rb;
			}
			else if (rb.bid == e.bid2) {
				pc2 = &rb;
			}

			if (pc1 && pc2)
				found = true;
		}
		auto& ccArray = scene.GetArray<CharacterController>();
		for (auto it = ccArray.begin(); it != ccArray.end() && !found; ++it) {

			CharacterController& cc = *it;
			if (cc.bid == e.bid1) {
				pc1 = &cc;
			}
			else if (cc.bid == e.bid2) {
				pc2 = &cc;
			}

			if (pc1 && pc2)
				found = true;
		}

		if (!pc1 || !pc2)
		continue;

		// Publish the right event
		if (e.op == EngineCollisionData::collisionOperation::added) {

			bool trigger = false;
			bool trigger2 = false;

			if (pc1->componentType == PhysicsComponent::Type::rb) {
				Rigidbody* tmp = reinterpret_cast<Rigidbody*>(pc1);
				if (tmp->is_trigger)
					trigger = true;
			}
			if (pc2->componentType == PhysicsComponent::Type::rb) {
				Rigidbody* tmp = reinterpret_cast<Rigidbody*>(pc2);
				if (tmp->is_trigger)
					trigger2 = true;
			}

			// Trigger or Collision
			if (trigger || trigger2) {
				TriggerEnterEvent tee;
				tee.pc1 = pc1;
				tee.pc2 = pc2;
				EVENTS.Publish(&tee);
				PRINT("Sending Trigger Enter Event\n");
			}
			else {
				ContactAddedEvent cae;
				cae.pc1 = pc1;
				cae.pc2 = pc2;
				EVENTS.Publish(&cae);
				//std::cout << "Collision Enter!\n";
			}
		}
		else if (e.op == EngineCollisionData::collisionOperation::removed) {
			bool trigger = false;
			bool trigger2 = false;

			if (pc1->componentType == PhysicsComponent::Type::rb) {
				Rigidbody* tmp = reinterpret_cast<Rigidbody*>(pc1);
				if (tmp->is_trigger)
					trigger = true;
			}
			if (pc2->componentType == PhysicsComponent::Type::rb) {
				Rigidbody* tmp = reinterpret_cast<Rigidbody*>(pc2);
				if (tmp->is_trigger)
					trigger2 = true;
			}
			// Trigger or Collision
			if (trigger || trigger2) {
				TriggerRemoveEvent tre;
				tre.pc1 = pc1;
				tre.pc2 = pc2;
				EVENTS.Publish(&tre);
				PRINT("Sending Trigger Remove Event\n");
			}
			else {
				ContactRemovedEvent cre;
				cre.pc1 = pc1;
				cre.pc2 = pc2;
				EVENTS.Publish(&cre);
				//PRINT("Sending Collision Remove Event\n");
			}
		}
		else if (e.op == EngineCollisionData::collisionOperation::persisted) {
			bool trigger = false;
			bool trigger2 = false;

			if (pc1->componentType == PhysicsComponent::Type::rb) {
				Rigidbody* tmp = reinterpret_cast<Rigidbody*>(pc1);
				if (tmp->is_trigger)
					trigger = true;
			}
			if (pc2->componentType == PhysicsComponent::Type::rb) {
				Rigidbody* tmp = reinterpret_cast<Rigidbody*>(pc2);
				if (tmp->is_trigger)
					trigger2 = true;
			}
			// Trigger or Collision
			if (trigger || trigger2) {
				TriggerStayEvent tse;
				tse.pc1 = pc1;
				tse.pc2 = pc2;
				EVENTS.Publish(&tse);
				//PRINT("Sending Trigger stay Event\n");

			}
			else {
				ContactStayEvent cse;
				cse.pc1 = pc1;
				cse.pc2 = pc2;
				EVENTS.Publish(&cse);
				//PRINT("Sending Collision stay Event\n");
			}
		}
	}
	engineContactListener->collisionResolution.clear();
	
	// Character collision tolerance
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
		if (cc.state == DELETED) continue;
		if (!scene.IsActive(cc)) continue;
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
		JPH::Vec3 directionNormalized;
		float length = 0.f;

		if (direction != JPH::Vec3::sZero()) {
			directionNormalized = direction.Normalized();
			length = direction.Length();
		}
		else {
			directionNormalized = JPH::Vec3::sZero();
			length = 0;
		}

		//std::cout << "direction:" << direction.GetX() << ',' << direction.GetY() << ',' << direction.GetZ() << std::endl;
		// Prevent character from going up steep slopes
		JPH::Character::EGroundState groundState = mCharacter->GetGroundState();
		if (groundState == JPH::Character::EGroundState::OnSteepGround
			|| groundState == JPH::Character::EGroundState::NotSupported)
		{
			//if(groundState == JPH::Character::EGroundState::OnSteepGround)
			//	PRINT("steep ground\n");

			//JPH::Vec3 normal = mCharacter->GetGroundNormal();
			//std::cout << "normal:" << normal.GetX() << ',' << normal.GetY() << ',' << normal.GetZ() << std::endl;

			//normal.SetY(0.0f);
			//float dot = normal.Dot(direction);
			//if (dot < 0.0f)
			//	directionNormalized -= (dot * normal) / normal.LengthSq();
		}

		// Update velocity
		if (mCharacter->IsSupported()) {
			//PRINT("supported\n");
			JPH::Vec3 current_velocity = mCharacter->GetLinearVelocity();
			JPH::Vec3 desired_velocity = directionNormalized * length;
			desired_velocity.SetY(current_velocity.GetY());
			JPH::Vec3 new_velocity = 0.75f * current_velocity + 0.25f * desired_velocity;

			// Jump
			if(groundState == JPH::Character::EGroundState::OnGround)
				new_velocity += JPH::Vec3(0, direction.GetY(), 0);
			//std::cout << "new velocity:" << new_velocity.GetX() << ',' << new_velocity.GetY() << ',' << new_velocity.GetZ() << std::endl;


			//PRINT("end\n");
			mCharacter->SetLinearVelocity(new_velocity);
		}

		// Reset character controller's direction
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
	//std::cout << "Physics System scene start test\n";

}
void PhysicsSystem::CallbackSceneStop(SceneStopEvent* pEvent) 
{
	UNREFERENCED_PARAMETER(pEvent);
	//std::cout << "Physics System scene stop test\n";
	//std::cout << "Num Bodies before: " << physicsSystem->GetNumBodies() << std::endl;
	//std::cout << "Num characters: " << characters.size() << std::endl;
	//Clean up any characters
	populated = false;
	for (JPH::Ref<JPH::Character>& r : characters) {

		if (r == nullptr)
			continue;

		r->RemoveFromPhysicsSystem();
		r = nullptr;
	}
	characters.clear();

	//std::cout << "Num characters: " << characters.size() << std::endl;

	//std::cout << "Num Bodies after: " << physicsSystem->GetNumBodies() << std::endl;


	// Delete the current physics system, must set to nullptr
	if (physicsSystem) {
		delete ccTest;
		ccTest = nullptr;
		delete physicsSystem;
		physicsSystem = nullptr;

	}

	engineContactListener->pSystem = nullptr;
}
void PhysicsSystem::CallbackObjectCreated(ObjectCreatedEvent<Rigidbody>* pEvent) {
	if (!pEvent || !pEvent->pObject)
		return;
	AddRigidBody(pEvent);
}
void PhysicsSystem::CallbackObjectDestroyed(ObjectDestroyedEvent<Rigidbody>* pEvent)
{
	if (!pEvent || !pEvent->pObject)
		return;
	DeleteBody(pEvent->pObject->bid);
}

void PhysicsSystem::PopulatePhysicsWorld() {

	if (!physicsSystem)
		return;
	populated = true;
	Scene& scene = MySceneManager.GetCurrentScene();

	// check entity for collider and then check what kind of collider he want
	// Shape Setting -> Shape Result -> Shape Refc -> Body Creation Setting -> Body
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
			
	
		Rigidbody& rb = *it;
		if (rb.state == DELETED) continue;
		Entity& entity = scene.Get<Entity>(rb);
		
		// Set enabled status
		JPH::EActivation enabledStatus = JPH::EActivation::Activate;
		if (!scene.IsActive(entity) || !scene.IsActive(rb)) {
			enabledStatus = JPH::EActivation::DontActivate;
			PRINT("set body to sleep first\n");
		}




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
			Vector3 colliderScale(boxCollider.dimensions.x * t.scale.x/2.f, boxCollider.dimensions.y * t.scale.y/2.f, boxCollider.dimensions.z * t.scale.z/2.f);
			GlmVec3ToJoltVec3(colliderScale, scale);

			Vector3 finalPos(t.translation.operator glm::vec3() + boxCollider.offset.operator glm::vec3());
			GlmVec3ToJoltVec3(finalPos, pos);

			//// Default motion type for trigger volumes should be static
			//if (rb.is_trigger)
			//{
			//	motionType = JPH::EMotionType::Static;
			//}

			JPH::BodyCreationSettings boxCreationSettings(new JPH::BoxShape(scale), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			
			if (rb.isStatic)
			{
				boxCreationSettings.mObjectLayer = EngineObjectLayers::STATIC;
			}


			// Set all necessary settings for the body
			// Friction
			boxCreationSettings.mFriction = rb.friction;
			// Linear Velocity
			boxCreationSettings.mLinearVelocity = linearVel;
			// Angular Velocity
			boxCreationSettings.mAngularVelocity = angularVel;
			// Sensor settings 
			boxCreationSettings.mIsSensor = rb.is_trigger;
			if (rb.is_trigger)
			{
				boxCreationSettings.mObjectLayer = EngineObjectLayers::SENSOR;
				boxCreationSettings.mSensorDetectsStatic = true;
				boxCreationSettings.mAllowSleeping = true;
			}

			boxCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			boxCreationSettings.mMassPropertiesOverride.mMass = 1.0f;
			// Create the actual jolt body
			JPH::Body* box = bodyInterface->CreateBody(boxCreationSettings);
			bodyInterface->AddBody(box->GetID(), enabledStatus);
			rb.bid = box->GetID().GetIndexAndSequenceNumber();

		}
		else if (scene.Has<SphereCollider>(entity)) {

			//std::cout << "sphere collider\n";
			SphereCollider& sc = scene.Get<SphereCollider>(entity);
			Vector3 finalPos(t.translation.operator glm::vec3() + sc.offset.operator glm::vec3());
			GlmVec3ToJoltVec3(finalPos, pos);

			float radius = (t.scale.x < t.scale.z ? t.scale.z : t.scale.x) * sc.radius;

			if (rb.is_trigger)
			{
				motionType = JPH::EMotionType::Kinematic;
			}

			JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
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
			if (rb.is_trigger)
			{
				sphereCreationSettings.mObjectLayer = EngineObjectLayers::SENSOR;
				sphereCreationSettings.mSensorDetectsStatic = true;
				sphereCreationSettings.mAllowSleeping = true;
			}

			sphereCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
			sphereCreationSettings.mMassPropertiesOverride.mMass = 1.0f;

			JPH::Body* sphere = bodyInterface->CreateBody(sphereCreationSettings);
			bodyInterface->AddBody(sphere->GetID(), enabledStatus);
			rb.bid = sphere->GetID().GetIndexAndSequenceNumber();
		}
		else if (scene.Has<CapsuleCollider>(entity)) {


			CapsuleCollider& cc = scene.Get<CapsuleCollider>(entity);	

			float radius = (t.scale.x < t.scale.z ? t.scale.z : t.scale.x) * cc.radius;
			float offset = 0.5f * (t.scale.y * cc.height) - radius;

			if (offset <= 0.f) {
				JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);

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
				if (rb.is_trigger)
				{
					sphereCreationSettings.mObjectLayer = EngineObjectLayers::SENSOR;
					sphereCreationSettings.mSensorDetectsStatic = true;
					sphereCreationSettings.mAllowSleeping = true;
				}

				sphereCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
				sphereCreationSettings.mMassPropertiesOverride.mMass = 1.0f;

				JPH::Body* sphere = bodyInterface->CreateBody(sphereCreationSettings);
				bodyInterface->AddBody(sphere->GetID(), enabledStatus);
				rb.bid = sphere->GetID().GetIndexAndSequenceNumber();
			}
			else {
				JPH::BodyCreationSettings capsuleCreationSettings(new JPH::CapsuleShape(offset, radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);

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
				if (rb.is_trigger)
				{
					capsuleCreationSettings.mObjectLayer = EngineObjectLayers::SENSOR;
					capsuleCreationSettings.mSensorDetectsStatic = true;
					capsuleCreationSettings.mAllowSleeping = true;
				}

				capsuleCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
				capsuleCreationSettings.mMassPropertiesOverride.mMass = 1.0f;

				JPH::Body* capsule = bodyInterface->CreateBody(capsuleCreationSettings);
				bodyInterface->AddBody(capsule->GetID(), enabledStatus);
				rb.bid = capsule->GetID().GetIndexAndSequenceNumber();
			}

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


	//std::cout << "Rigido bodios:" << scene.GetArray<Rigidbody>().size() << std::endl;

	//std::cout << "Number of jolt bodies:" << physicsSystem->GetNumActiveBodies(JPH::EBodyType::RigidBody) << std::endl;

}

void PhysicsSystem::UpdateGameObjects() {

	if (!physicsSystem)
		return;

	Scene& scene = MySceneManager.GetCurrentScene();

	// Rigidbodies
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		Rigidbody& rb = *it;
		if (rb.state == DELETED)
			continue;
		if (!it.IsActive())
			continue;
		Entity& entity = scene.Get<Entity>(rb);
		if (!scene.IsActive(entity))
			continue;

		Transform& t = scene.Get<Transform>(entity);


		Vector3 tmpVec;
		JPH::BodyID tmpBID(rb.bid);
		JPH::RVec3 tmp = bodyInterface->GetCenterOfMassPosition(tmpBID);
		JoltVec3ToGlmVec3(tmp, tmpVec);	
		if (scene.Has<BoxCollider>(entity)) {
			t.translation = static_cast<Vector3>(tmpVec.operator glm::vec3() - scene.Get<BoxCollider>(entity).offset.operator glm::vec3());

		}
		else if (scene.Has<SphereCollider>(entity)) {
			t.translation = static_cast<Vector3>(tmpVec.operator glm::vec3() - scene.Get<SphereCollider>(entity).offset.operator glm::vec3());
		}
		//t.translation = tmpVec;

		JPH::Quat tmpQuat = bodyInterface->GetRotation(tmpBID);
		JoltQuatToGlmVec3(tmpQuat, tmpVec);
		t.rotation = tmpVec;

		tmp = bodyInterface->GetLinearVelocity(tmpBID);
		JoltVec3ToGlmVec3(tmp, rb.linearVelocity);

		tmp = bodyInterface->GetAngularVelocity(tmpBID);
		JoltVec3ToGlmVec3(tmp, rb.angularVelocity);

	}

	// Character Controllers
	size_t idx = 0;
	auto& ccArray = scene.GetArray<CharacterController>();
	for (auto it = ccArray.begin(); it != ccArray.end(); ++it) {

		if (idx >= characters.size())
			break;

		CharacterController& cc = *it;
		Entity& entity = scene.Get<Entity>(cc);
		Transform& t = scene.Get<Transform>(entity);

		JPH::BodyID tmpBID(cc.bid);
		JPH::RVec3 tmp = characters[idx]->GetLinearVelocity();
		JoltVec3ToGlmVec3(tmp, cc.velocity);

		tmp = characters[idx]->GetCenterOfMassPosition();
		JoltVec3ToGlmVec3(tmp, t.translation);
		if (characters[idx]->GetGroundState() == JPH::Character::EGroundState::OnGround)
		{
			//std::cout << "Character " << idx << " is grounded\n";
			cc.isGrounded = true;
		}
		else
		{
			cc.isGrounded = false;
		}

		idx++;
	}
}

void PhysicsSystem::DeleteBody(PhysicsComponent& pc) {
	if (!physicsSystem)
		return;

	//std::cout << "Num Bodies before: " << physicsSystem->GetNumBodies() << std::endl;

	if (pc.componentType == PhysicsComponent::Type::cc) {
		//std::cout << "getting rid of jolt character\n";

		//if (ccTest->mCharacter->GetBodyID().GetIndexAndSequenceNumber() == pc.bid) {
		//	ccTest->mCharacter->RemoveFromPhysicsSystem();
		//	ccTest->mCharacter = nullptr;
		//	PRINT("Number of Jolt Bodies after: " + physicsSystem->GetNumBodies());

		//	return;
		//}

		for (JPH::Ref<JPH::Character>& r : characters) {
			if (r->GetBodyID().GetIndexAndSequenceNumber() == pc.bid) {
				r->RemoveFromPhysicsSystem();
				r = nullptr;				
				return;
			}

		}
	}
	else {

		bodyInterface->RemoveBody(JPH::BodyID(pc.bid));

	}

	//std::cout << "Num Bodies after: " << physicsSystem->GetNumBodies() << std::endl;


}
void PhysicsSystem::DeleteBody(UINT32 bid) {

	if (!physicsSystem)
		return;

	//std::cout << "Deleting Body!\n";
	//std::cout << "Num Bodies before: " << physicsSystem->GetNumBodies() << std::endl;

	if (ccTest->mCharacter && ccTest->mCharacter->GetBodyID().GetIndexAndSequenceNumber() == bid) {
		ccTest->mCharacter->RemoveFromPhysicsSystem();
		bodyInterface->DestroyBody(ccTest->mCharacter->GetBodyID());
		ccTest->mCharacter = nullptr;

		//std::cout << "Num Bodies after: " << physicsSystem->GetNumBodies() << std::endl;

		return;
	}


	for (JPH::Ref<JPH::Character>& r : characters) {
		if (r->GetBodyID().GetIndexAndSequenceNumber() == bid) {

			r->RemoveFromPhysicsSystem();
			r = nullptr;
			//PRINT("Number of Jolt Bodies after: " + physicsSystem->GetNumBodies());

			return;
		}

	}
	

	bodyInterface->RemoveBody(JPH::BodyID(bid));
	//std::cout << "Num Bodies after: " << physicsSystem->GetNumBodies() << std::endl;
}

void PhysicsSystem::AddRigidBody(ObjectCreatedEvent<Rigidbody>* pEvent) {

	Scene& scene = MySceneManager.GetCurrentScene();
	if (!populated)
		return;
	if (!pEvent || !pEvent->pObject)
		return;
	if (!physicsSystem)
		return;

	Rigidbody& rb = *(pEvent->pObject);

	if (rb.state == DELETED)
		return;

	Entity& entity = scene.Get<Entity>(rb);

	// Set enabled status
	JPH::EActivation enabledStatus = JPH::EActivation::Activate;
	if (!scene.IsActive(entity) || !scene.IsActive(rb)) {
		enabledStatus = JPH::EActivation::DontActivate;
	}
	// If no collider is attached with the rigidbody, reject gameobject
	if (!scene.Has<BoxCollider>(entity) && !scene.Has<SphereCollider>(entity) && !scene.Has<CapsuleCollider>(entity))
	{
		return;
	}
	// Position, Rotation and Scale of collider
	Transform& t = scene.Get<Transform>(entity);

	Engine::UUID parent = t.parent;

	//Ensure parents are created in the scene
	while (parent != 0)
	{
		if (!scene.HasHandle<Transform>({ parent,0 }))
		{
			return;
		}
		Transform& parentTrans = scene.Get<Transform>(parent);
		parent = parentTrans.parent;
	}
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
		Vector3 colliderScale(boxCollider.dimensions.x * t.scale.x / 2.f, boxCollider.dimensions.y * t.scale.y / 2.f, boxCollider.dimensions.z * t.scale.z / 2.f);
		GlmVec3ToJoltVec3(colliderScale, scale);

		Vector3 finalPos(t.translation.operator glm::vec3() + boxCollider.offset.operator glm::vec3());
		GlmVec3ToJoltVec3(finalPos, pos);

		if (rb.is_trigger)
		{
			motionType = JPH::EMotionType::Kinematic;
		}

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
		if (rb.is_trigger)
		{
			boxCreationSettings.mObjectLayer = EngineObjectLayers::SENSOR;
			boxCreationSettings.mSensorDetectsStatic = true;
		}

		boxCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
		boxCreationSettings.mMassPropertiesOverride.mMass = 1.0f;
		// Create the actual jolt body
		JPH::Body* box = bodyInterface->CreateBody(boxCreationSettings);
		bodyInterface->AddBody(box->GetID(), enabledStatus);
		rb.bid = box->GetID().GetIndexAndSequenceNumber();

	}
	else if (scene.Has<SphereCollider>(entity)) {

		std::cout << "sphere collider\n";
		SphereCollider& sc = scene.Get<SphereCollider>(entity);
		Vector3 finalPos(t.translation.operator glm::vec3() + sc.offset.operator glm::vec3());
		GlmVec3ToJoltVec3(finalPos, pos);

		float radius = (t.scale.x < t.scale.z ? t.scale.z : t.scale.x) * sc.radius;

		if (rb.is_trigger)
		{
			motionType = JPH::EMotionType::Kinematic;
		}

		JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape( radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
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
		if (rb.is_trigger)
		{
			sphereCreationSettings.mObjectLayer = EngineObjectLayers::SENSOR;
			sphereCreationSettings.mSensorDetectsStatic = true;
		}

		sphereCreationSettings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
		sphereCreationSettings.mMassPropertiesOverride.mMass = 1.0f;

		JPH::Body* sphere = bodyInterface->CreateBody(sphereCreationSettings);
		bodyInterface->AddBody(sphere->GetID(), enabledStatus);
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
	characterSetting->mUp = JPH::Vec3(0,1,0);

	float radius = (t.scale.x < t.scale.z ? t.scale.z : t.scale.x) * cc.radius;
	float offset = 0.5f * (t.scale.y * cc.height) - radius;

	// Character Shape (default capsule)
	JPH::RefConst<JPH::Shape> capsuleCharacterShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, 0, 0),
													JPH::Quat::sIdentity(), new JPH::CapsuleShape(offset, radius)).Create().Get();
	
	JPH::RefConst<JPH::Shape> sphereCharacterShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3(0, 0, 0),
													JPH::Quat::sIdentity(), new JPH::SphereShape(radius)).Create().Get();
	if (offset <= 0.0f) {
		characterSetting->mShape = sphereCharacterShape;
		PRINT("character uses sphere collider\n");
	}
	else {
		characterSetting->mShape = capsuleCharacterShape;
		PRINT("character uses capsule collider\n");
	}

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
	cc.componentType = PhysicsComponent::Type::cc;

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
	(void)ioSettings;
	if (!pSystem)
		return;
	// If bodies already have contact, do not register another OnCollisionEnter/OnTriggerEnter
	if (pSystem->WereBodiesInContact(body1.GetID(), body2.GetID())) {
		//std::cout << "reject added contact\n";
		return;
	}

	/*DEBUG
	//Scene& scene = MySceneManager.GetCurrentScene();

	// Rigidbodies
	//auto& rbArray = scene.GetArray<Rigidbody>();

	//for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
	//	Rigidbody& rb = *it;
	//	if (rb.state == DELETED)
	//		continue;
	//	if (!it.IsActive())
	//		continue;
	//	Entity& entity = scene.Get<Entity>(rb);
	//	if (!scene.IsActive(entity))
	//		continue;
	//	if (body1.GetID().GetIndexAndSequenceNumber() == rb.bid)
	//	{
	//		PRINT("Body1 Contact Added: ", scene.Get<Tag>(rb).name);
	//	}
	//	else if (body2.GetID().GetIndexAndSequenceNumber() == rb.bid)
	//	{
	//		PRINT("Body2 Contact Added: ", scene.Get<Tag>(rb).name);
	//	}
	//}*/

	if (!body1.IsActive() && !body1.IsStatic() || !body2.IsActive() && !body2.IsStatic()) {
		PRINT("no contact added as one of the bodies are sleeping\n");
		return;
	}

	JPH::RVec3 p1 = manifold.GetWorldSpaceContactPointOn1(0);
	JPH::RVec3 p2 = manifold.GetWorldSpaceContactPointOn2(0);

	Vector3 vp1;
	Vector3 vp2;

	JoltVec3ToGlmVec3(p1, vp1);
	JoltVec3ToGlmVec3(p2, vp2);


	ACQUIRE_SCOPED_LOCK(PhysicsCollision);
	collisionResolution.emplace_back(EngineCollisionData(EngineCollisionData::collisionOperation::added));
	collisionResolution.back().bid1 = body1.GetID().GetIndexAndSequenceNumber();
	collisionResolution.back().bid2 = body2.GetID().GetIndexAndSequenceNumber();
	collisionResolution.back().p1 = vp1;
	collisionResolution.back().p1 = vp2;

	std::cout << vp1.x << "|" << vp1.y << "|" << vp1.z << std::endl;
	std::cout << vp2.x << "|" << vp2.y << "|" << vp2.z << std::endl;


	//std::cout << "Contact Added\n";
}
void EngineContactListener::OnContactPersisted(const JPH::Body& body1, const JPH::Body& body2, const JPH::ContactManifold& manifold, JPH::ContactSettings& ioSettings) 
{
	(void)manifold;
	(void)ioSettings;
	//if (pSystem->WereBodiesInContact(body1.GetID(), body2.GetID()))
	//	return;	
	
	//if(body1.IsSensor() || body2.IsSensor())
		//std::cout << "Contact persisting!\n";


	ACQUIRE_SCOPED_LOCK(PhysicsCollision);
	collisionResolution.emplace_back(EngineCollisionData(EngineCollisionData::collisionOperation::persisted));
	collisionResolution.back().bid1 = body1.GetID().GetIndexAndSequenceNumber();
	collisionResolution.back().bid2 = body2.GetID().GetIndexAndSequenceNumber();
}
void EngineContactListener::OnContactRemoved(const JPH::SubShapeIDPair& subShapePair) {
	if (!pSystem)
		return;

	// If the bodies are still touching, do not register OnCollisionExit/OnTriggerExit
	if (pSystem->WereBodiesInContact(subShapePair.GetBody1ID(), subShapePair.GetBody2ID())) {
		return;
	}
	//std::cout << "Contact Removed\n";

	ACQUIRE_SCOPED_LOCK(PhysicsCollision);
	collisionResolution.emplace_back(EngineCollisionData(EngineCollisionData::collisionOperation::removed));
	collisionResolution.back().bid1 = subShapePair.GetBody1ID().GetIndexAndSequenceNumber();
	collisionResolution.back().bid2 = subShapePair.GetBody2ID().GetIndexAndSequenceNumber();
}
#pragma endregion

#pragma region MathConversionHelpers
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
#pragma endregion
