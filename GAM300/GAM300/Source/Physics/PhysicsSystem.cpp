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
#include "IOManager/InputSystem.h"

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

namespace
{
	template <typename T>
	glm::mat4 GetColliderLocalTransform(const T& col)
	{
		return glm::mat4();
	}

	template <>
	glm::mat4 GetColliderLocalTransform<BoxCollider>(const BoxCollider& boxCol)
	{
		return Transform::CreateTransformationMtx(boxCol.offset, vec3(0), (glm::vec3)boxCol.dimensions/2.f);
	}

	template <>
	glm::mat4 GetColliderLocalTransform<SphereCollider>(const SphereCollider& sphereCol)
	{
		return Transform::CreateTransformationMtx(sphereCol.offset, vec3(0), vec3(sphereCol.radius));
	}

	template <>
	glm::mat4 GetColliderLocalTransform<CapsuleCollider>(const CapsuleCollider& capsuleCol)
	{
		return Transform::CreateTransformationMtx(vec3(0), vec3(0), vec3(capsuleCol.radius));
	}



	template <typename T>
	glm::mat4 GetColliderGlobalTransform(const Transform& transform, const  T& col)
	{
		return transform.GetWorldMatrix() * GetColliderLocalTransform<T>(col);
	}

	template <typename T>
	glm::mat4 GetColliderLocalOffset(T& col)
	{
		return Transform::CreateTransformationMtx(col.offset, vec3(0), vec3(1));
	}

	template <typename T, typename... Ts>
	glm::mat4 GetColliderLocalOffset(Scene& scene, Entity& entity)
	{
		if (entity.HasComponent<T>())
		{
			return GetColliderLocalOffset<T>(scene.Get<T>(entity));
		}

		if constexpr (sizeof...(Ts) != 0)
			GetColliderLocalOffset<Ts...>(scene, entity);
	}

	template <typename... Ts>
	glm::mat4 GetColliderLocalOffset(Scene& scene, Entity& entity, TemplatePack<Ts...>)
	{
		return GetColliderLocalOffset<Ts...>(scene, entity);
	}

	glm::mat4 GetColliderLocalOffset(Scene& scene, Entity& entity)
	{
		return GetColliderLocalOffset(scene, entity, ColliderComponentTypes());
	}

	template <typename T>
	glm::mat4 GetColliderGlobalOffset(Transform& transform, T& col)
	{
		return transform.GetWorldMatrix() * GetColliderLocalOffset<T>(col);
	}

	template <typename T,typename... Ts>
	static void UpdateBodyTransform(Scene& scene, Entity& entity,Transform& t,JPH::BodyID bid,JPH::BodyInterface* bodyInterface)
	{
		//Iterate through collider types
		if (entity.HasComponent<T>())
		{
			glm::mat4 transMtx = GetColliderGlobalOffset(t, scene.Get<T>(t));
			JPH::Vec3 translation;
			JPH::Quat rot;
			Vector3 scale;
			Transform::Decompose(transMtx, (Vector3&)translation, (glm::quat&)rot, (Vector3&)scale);
			bodyInterface->SetPositionAndRotation(bid, translation, rot, JPH::EActivation::DontActivate);
			return;
		}


		if constexpr (sizeof...(Ts) != 0)
			UpdateBodyTransform<Ts...>(scene, entity, t, bid, bodyInterface);
	}

	template <typename... Ts>
	static void UpdateBodyTransform(Scene& scene, Entity& entity, Transform& t, JPH::BodyID bid, JPH::BodyInterface* bodyInterface, TemplatePack<Ts...>)
	{
		UpdateBodyTransform<Ts...>(scene, entity, t, bid, bodyInterface);
	}

	inline static void UpdateBodyTransform(Scene& scene, Entity& entity, JPH::BodyID bid, JPH::BodyInterface* bodyInterface)
	{
		Transform& t = scene.Get<Transform>(entity);
		//If unmodified, do nothing
		if (!t.GetFlag(Transform::Flag::Modified))
			return;
		UpdateBodyTransform(scene, entity, t, bid, bodyInterface, ColliderComponentTypes());
	}

}

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


void PhysicsSystem::UpdateJoltTransforms()
{
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& rbArray = scene.GetArray<Rigidbody>();
	for (auto it = rbArray.begin(); it != rbArray.end(); ++it) {
		if (!it.IsActive())
		{
			continue;
		}
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);
		if (!scene.IsActive(entity))
		{
			continue;
		}

		JPH::BodyID tmpBID(rb.bid);

		UpdateBodyTransform(scene, entity, tmpBID, bodyInterface);

		bodyInterface->SetLinearVelocity(tmpBID, (JPH::Vec3&)rb.linearVelocity);
		bodyInterface->SetAngularVelocity(tmpBID, (JPH::Vec3&)rb.angularVelocity);
		bodyInterface->AddForce(tmpBID, (JPH::Vec3&)rb.force);
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
					lock.ReleaseLock();
					bodyInterface->DeactivateBody(tmpBID);
				}

			}

		}
	}

	auto& ccArray = scene.GetArray<CharacterController>();
	int j = 0;
	for (auto it = ccArray.begin(); it != ccArray.end(); ++it) {
		CharacterController& cc = *it;
		JPH::BodyID tmpBid{ cc.bid };

		Transform& t = scene.Get<Transform>(cc);
		JPH::RVec3 pos;
		Vector3 tpos = t.GetGlobalTranslation();
		GlmVec3ToJoltVec3(tpos, pos);
		JPH::Quat rot;
		Vector3 trot = t.GetGlobalRotation();
		GlmVec3ToJoltQuat(trot, rot);

		JPH::RVec3 velocity;
		GlmVec3ToJoltVec3(cc.velocity, velocity);

		bodyInterface->SetPosition(tmpBid, pos, JPH::EActivation::DontActivate);
		bodyInterface->SetRotation(tmpBid, rot, JPH::EActivation::DontActivate);
		bodyInterface->SetLinearVelocity(tmpBid, velocity);
		++j;
	}
}


void PhysicsSystem::Update(float dt) {

	if (!physicsSystem)
		return;

	UpdateJoltTransforms();
	
	// Fixed time steps
	if (physicsSystem) {

		float fixedDt = (float)MyFrameRateController.GetFixedDt();
		for (int i = 0; i < MyFrameRateController.GetSteps(); ++i)
		{
			PrePhysicsUpdate(dt);
			physicsSystem->Update(fixedDt, 1, tempAllocator, jobSystem);
			step++;
		}
		/*PrePhysicsUpdate(dt);

		physicsSystem->Update(dt, 1, tempAllocator, jobSystem);*/

	}

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
	//// Test raycast
	//if (InputHandler::isKeyButtonPressed(GLFW_KEY_T)) {
	//	std::cout << "Casting Ray\n";
	//	JPH::Vec3 pt = CastRay(JPH::Vec3(0, -50, 0), JPH::Vec3(0,300,0), 100.f);
	//}
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
				//PRINT("Sending Trigger Enter Event\n");
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
	// Clear engine collisions
	engineContactListener->collisionResolution.clear();
	
	// Character collision tolerance
	for (auto it = characters.begin(); it != characters.end(); ++it) {
		(*it)->PostSimulation(0.05f);
	}

	// Update game objects to their corresponding physics bodies
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
	physicsSystem->OptimizeBroadPhase();

	PopulatePhysicsWorld();
	//std::cout << "Physics System scene start test\n";

}
void PhysicsSystem::CallbackSceneStop(SceneStopEvent* pEvent) 
{
	UNREFERENCED_PARAMETER(pEvent);

	//Clean up any characters
	populated = false;
	for (JPH::Ref<JPH::Character>& r : characters) {

		if (r == nullptr)
			continue;

		r->RemoveFromPhysicsSystem();
		r = nullptr;
	}
	characters.clear();


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
		Vector3 tpos = t.GetGlobalTranslation();
		GlmVec3ToJoltVec3(tpos, pos);
		JPH::Quat rot;
		Vector3 trot = t.GetGlobalRotation();
		GlmVec3ToJoltQuat(trot, rot);
		Vector3 tscale = t.GetGlobalScale();

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

		/*
		// Create rigidbody's collider shape
		if (scene.Has<BoxCollider>(entity)) {

			BoxCollider& boxCollider = scene.Get<BoxCollider>(entity);
			Vector3 tScale = t.GetGlobalScale();
			Vector3 tPos = t.GetGlobalTranslation();
			Vector3 colliderScale(boxCollider.dimensions.x * tScale.x/2.f, boxCollider.dimensions.y * tScale.y/2.f, boxCollider.dimensions.z * tScale.z/2.f);
			GlmVec3ToJoltVec3(colliderScale, scale);

			Vector3 finalPos(tPos.operator glm::vec3() + boxCollider.offset.operator glm::vec3());
			GlmVec3ToJoltVec3(finalPos, pos);

			JPH::BodyCreationSettings boxCreationSettings(new JPH::BoxShape(scale), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			SetBodyCreationSettings(boxCreationSettings, rb, enabledStatus);


		}
		else if (scene.Has<SphereCollider>(entity)) {

			Vector3 tScale = t.GetGlobalScale();
			SphereCollider& sc = scene.Get<SphereCollider>(entity);
			Vector3 finalPos(tpos.operator glm::vec3() + sc.offset.operator glm::vec3());
			GlmVec3ToJoltVec3(finalPos, pos);

			float radius = (tScale.x < tScale.z ? tScale.z : tScale.x) * sc.radius;

			if (rb.is_trigger)
			{
				motionType = JPH::EMotionType::Kinematic;
			}

			JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			SetBodyCreationSettings(sphereCreationSettings, rb, enabledStatus);

		}
		else if (scene.Has<CapsuleCollider>(entity)) {


			CapsuleCollider& cc = scene.Get<CapsuleCollider>(entity);
			Vector3 tScale = t.GetGlobalScale();
			GlmVec3ToJoltVec3(tpos, pos);


			float radius = (tScale.x < tScale.z ? tScale.z : tScale.x) * cc.radius;
			float offset = 0.5f * (tScale.y * cc.height) - radius;

			if (offset <= 0.f) {
				JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);

				SetBodyCreationSettings(sphereCreationSettings, rb, enabledStatus);

			}
			else {
				JPH::BodyCreationSettings capsuleCreationSettings(new JPH::CapsuleShape(offset, radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
				SetBodyCreationSettings(capsuleCreationSettings, rb, enabledStatus);

			}

		}
		else {
			continue;
		}*/

		// Create rigidbody's collider shape
		if (scene.Has<BoxCollider>(entity)) {

			BoxCollider& boxCollider = scene.Get<BoxCollider>(entity);

			glm::mat4 boxTransMtx = GetColliderGlobalTransform(t, boxCollider);

			Transform::Decompose(boxTransMtx, (Vector3&)pos, (glm::quat&)rot, (Vector3&)scale);

			PRINT(scene.Get<Tag>(boxCollider).name,'\n');

			JPH::BodyCreationSettings boxCreationSettings(new JPH::BoxShape(scale), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			SetBodyCreationSettings(boxCreationSettings, rb, enabledStatus);

		}
		else if (scene.Has<SphereCollider>(entity)) {

			SphereCollider& sc = scene.Get<SphereCollider>(entity);

			// Calculate sphere collider radius
			float radius = (tscale.x < tscale.z ? tscale.z : tscale.x) * sc.radius;

			// Account for offset to get final position of collider
			Vector3 finalPos(tpos.operator glm::vec3() + sc.offset.operator glm::vec3());
			GlmVec3ToJoltVec3(finalPos, pos);

			JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
			SetBodyCreationSettings(sphereCreationSettings, rb, enabledStatus);

		}
		else if (scene.Has<CapsuleCollider>(entity)) {


			CapsuleCollider& cc = scene.Get<CapsuleCollider>(entity);

			float radius = (tscale.x < tscale.z ? tscale.z : tscale.x) * cc.radius;
			float offset = 0.5f * (tscale.y * cc.height);
			if (cc.height >= 0.0f) {
				JPH::BodyCreationSettings capsuleCreationSettings(new JPH::CapsuleShape(offset, radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
				SetBodyCreationSettings(capsuleCreationSettings, rb, enabledStatus);
			}
			else {
				JPH::BodyCreationSettings sphereCreationSettings(new JPH::SphereShape(radius), pos, rot, motionType, EngineObjectLayers::DYNAMIC);
				SetBodyCreationSettings(sphereCreationSettings, rb, enabledStatus);
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

	size_t numBodies = rbArray.size() + ccArray.size();

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
		if (!it.IsActive())
			continue;
		Rigidbody& rb = *it;
		Entity& entity = scene.Get<Entity>(rb);
		if (!scene.IsActive(entity))
			continue;

		if (rb.isStatic) // Ignore Static object
			continue;

		Transform& t = scene.Get<Transform>(entity);

		JPH::BodyID tmpBID(rb.bid);

		JPH::Vec3 jphPos;
		JPH::Quat jphRot;
		Vector3 scale;

		//Get physics body position and rotation
		bodyInterface->GetPositionAndRotation(tmpBID, jphPos, jphRot);

		JPH::Vec3 jphRotEuler = jphRot.GetEulerAngles();

		Vector3 pos;
		Vector3 rotEuler;
		glm::quat rot;

		JoltVec3ToGlmVec3(jphPos, pos);
		JoltVec3ToGlmVec3(jphRotEuler, rotEuler);

		glm::mat4 bodyMtx = Transform::CreateTransformationMtx(pos, rotEuler, t.GetGlobalScale());

		glm::mat4 mtx = GetColliderLocalOffset(scene,entity);

		glm::mat4 entityMtx = bodyMtx * glm::inverse(mtx);

		Transform::Decompose(entityMtx, pos, rot, scale);

		t.SetWorldMatrix(pos, rotEuler, t.GetGlobalScale());

		JPH::Vec3 angVel;
		JPH::Vec3 lineVel;
		bodyInterface->GetLinearAndAngularVelocity(tmpBID,lineVel, angVel);

		JoltVec3ToGlmVec3(lineVel, rb.linearVelocity);
		JoltVec3ToGlmVec3(angVel, rb.angularVelocity);
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
		Vector3 newPos = t.GetGlobalTranslation();
		JoltVec3ToGlmVec3(tmp, newPos);
		t.SetGlobalPosition(newPos);
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
		PRINT("no collider is attached to this rigidbody, hence it is rejected\n");
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

	// Transform
	Vector3 scale;
	Vector3 pos;
	Vector3 rot;
	glm::quat rotQuat;

	JPH::Vec3 jphScale;
	JPH::Vec3 jphPos;
	JPH::Quat jphRot;


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
	if (rb.is_trigger)
		motionType = JPH::EMotionType::Kinematic;

	JPH::ConvexShape * shape = nullptr;
	glm::mat4 transMtx{};

	// Create rigidbody's collider shape
	if (scene.Has<BoxCollider>(entity)) {
		BoxCollider& boxCollider = scene.Get<BoxCollider>(entity);

		transMtx = GetColliderGlobalTransform(t, boxCollider);

		Transform::Decompose(transMtx, pos, rotQuat, scale);
		GlmVec3ToJoltVec3(scale, jphScale);

		shape = new JPH::BoxShape(jphScale);
	}
	else if (scene.Has<SphereCollider>(entity)) {
		SphereCollider& sc = scene.Get<SphereCollider>(entity);

		transMtx = GetColliderGlobalTransform(t, sc);

		Transform::Decompose(transMtx, pos, rotQuat, scale);
		GlmVec3ToJoltVec3(scale, jphScale);

		shape = new JPH::SphereShape(jphScale[jphScale.GetLowestComponentIndex()]);
	}
	else if (scene.Has<CapsuleCollider>(entity)) {
		CapsuleCollider& cc = scene.Get<CapsuleCollider>(entity);

		transMtx = GetColliderGlobalTransform(t, cc);

		Transform::Decompose(transMtx, pos, rotQuat, scale);
		GlmVec3ToJoltVec3(scale, jphScale);

		float radius = jphScale[jphScale.GetLowestComponentIndex()];
		float offset = 0.5f * (jphScale.GetY() * cc.height) - radius;
		if (offset <= 0.0f)
			shape = new JPH::CapsuleShape(offset, radius);
		else
			shape = new JPH::SphereShape(radius);
	}
	GlmVec3ToJoltVec3(pos, jphPos);
	GlmVec3ToJoltQuat(rot, jphRot);
	JPH::BodyCreationSettings bodyCreationSettings(shape, jphPos, jphRot, motionType, EngineObjectLayers::DYNAMIC);
	SetBodyCreationSettings(bodyCreationSettings, rb, enabledStatus);
}

// Create and add a Jolt Character to a Jolt physics system using a character controller
void CreateJoltCharacter(CharacterController& cc, JPH::PhysicsSystem* psystem, PhysicsSystem* enginePSystem) {

	Scene& scene = MySceneManager.GetCurrentScene();
	Entity& entity = scene.Get<Entity>(cc);
	Transform& t = scene.Get<Transform>(entity);

	Vector3 tScale = t.GetGlobalScale();
	JPH::Ref<JPH::CharacterSettings> characterSetting = new JPH::CharacterSettings;
	characterSetting->mMass = cc.mass;
	characterSetting->mFriction = cc.friction;
	characterSetting->mGravityFactor = cc.gravityFactor;
	characterSetting->mLayer = EngineObjectLayers::DYNAMIC;
	characterSetting->mMaxSlopeAngle = (cc.slopeLimit / 180.f) * 3.14f;	// converting to radian first
	characterSetting->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), -tScale.x);
	characterSetting->mUp = JPH::Vec3(0,1,0);

	float radius = (tScale.x < tScale.z ? tScale.z : tScale.x) * cc.radius;
	float offset = 0.5f * (tScale.y * cc.height) - radius;

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

	// Allow setting position through scripting, do not remove!!
	Vector3 tPos = t.GetGlobalTranslation();
	Vector3 tRot = t.GetGlobalRotation();
	JPH::RVec3 pos;
	GlmVec3ToJoltVec3(tPos, pos);
	JPH::Quat rot;
	GlmVec3ToJoltQuat(tRot, rot);
	t.SetGlobalPosition(tPos);
	t.SetGlobalRotation(tRot);


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

void PhysicsSystem::SetBodyCreationSettings(JPH::BodyCreationSettings& bcs, Rigidbody& rb, JPH::EActivation enabledStatus) {
	
	// Linear + Angular Velocity
	JPH::RVec3 linearVel;
	GlmVec3ToJoltVec3(rb.linearVelocity, linearVel);
	JPH::RVec3 angularVel;
	GlmVec3ToJoltVec3(rb.angularVelocity, angularVel);

	if (rb.isStatic)
		bcs.mObjectLayer = EngineObjectLayers::STATIC;

	// Set all necessary settings for the body
	// Friction
	bcs.mFriction = rb.friction;
	// Linear Velocity
	bcs.mLinearVelocity = linearVel;
	// Angular Velocity
	bcs.mAngularVelocity = angularVel;
	// Sensor settings
	bcs.mIsSensor = rb.is_trigger;
	if (rb.is_trigger)
	{
		bcs.mObjectLayer = EngineObjectLayers::SENSOR;
		bcs.mSensorDetectsStatic = true;
	}

	bcs.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
	bcs.mMassPropertiesOverride.mMass = 1.0f;


	JPH::Body* body = bodyInterface->CreateBody(bcs);
	bodyInterface->AddBody(body->GetID(), enabledStatus);
	rb.bid = body->GetID().GetIndexAndSequenceNumber();
	//std::cout << "add\n";
}

EngineRayCastResult PhysicsSystem::CastRay(JPH::RVec3& origin, const JPH::Vec3& direction, const float& distance) {
	
	Vector3 tmp;
	JoltVec3ToGlmVec3(origin, tmp);

	if (!physicsSystem)
		return EngineRayCastResult(Tag(), tmp, false);

	// TODO:
	/*
	* Normalise direction first then apply distance?
	* then no need to check for other stuff just get the 2nd last contact pt?
	*/


	JPH::AllHitCollisionCollector<JPH::RayCastBodyCollector> collector;

	const JPH::BroadPhaseQuery& bpq = physicsSystem->GetBroadPhaseQuery();
	JPH::RayCastSettings rcs;
	rcs.mBackFaceMode = JPH::EBackFaceMode::CollideWithBackFaces;
	JPH::RayCast ray(origin, direction * distance);
	bpq.CastRay(ray, collector);
	if(!collector.HadHit())
		return EngineRayCastResult(Tag(), tmp, false);
	size_t numHits = (int)collector.mHits.size();

	collector.Sort();
	JPH::BroadPhaseCastResult* results = collector.mHits.data();

	UINT32 bid;
	Tag tag;
	Vector3 hitPt = { 0.f, 0.f, 0.f };
	Scene& scene = MySceneManager.GetCurrentScene();
	auto& rbArray = scene.GetArray<Rigidbody>();
	bool selected = false;
	for (size_t i = 0; i < numHits; i++)
	{
		selected = false;
		bid = results[i].mBodyID.GetIndexAndSequenceNumber();
		JPH::RVec3 v = ray.GetPointOnRay(results[i].mFraction);
		
		for (auto it = rbArray.begin(); it != rbArray.end(); ++it) 
		{
			Rigidbody& rb = *it;
			if (rb.bid == bid && !rb.is_trigger) 
			{
				JoltVec3ToGlmVec3(v, hitPt);
				tag = scene.Get<Tag>(rb);
				if (tag.physicsLayerIndex != 1) // If it isnt on TransparentFX layer
				{
					selected = true;
					break;
				}
			}
		}

		if (selected)
			break;
	}


	

	//collector.Sort();
	/*
	for (int i{ 0 }; i < numHits; ++i) {
		
		JPH::Vec3 pt = ray.GetPointOnRay(results[i].mFraction);
		
		std::cout << "Contact pt: " << pt.GetX() << '|' << pt.GetY() << '|' << pt.GetZ() << std::endl;

		// Find 1st contact pt outside of max distance
		float distance = (pt - ray.mOrigin).Length();
		if (distance >= maxDistance && i >= 1) {

			closestPointToEnd = ray.GetPointOnRay(results[i-1].mFraction);
			break;
			
		}

		if (i == numHits - 1) {
			closestPointToEnd = ray.GetPointOnRay(results[i].mFraction);
			break;
		}
	}*/
	/*std::cout << "Closest pt " << tag.name << ": " << hitPt.x << '|'
									<< hitPt.y << '|' 
									<< hitPt.z << std::endl;*/
	return EngineRayCastResult(tag, hitPt, selected);
	
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

	//std::cout << vp1.x << "|" << vp1.y << "|" << vp1.z << std::endl;
	//std::cout << vp2.x << "|" << vp2.y << "|" << vp2.z << std::endl;


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
