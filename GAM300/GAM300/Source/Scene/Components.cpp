/*!***************************************************************************************
\file			Components.cpp
\project
\author			Zacharie Hong

\par			Course: GAM300
\par			Section:
\date			10/03/2023

\brief
	This file defines all types of components to be used by ECS

All content � 2023 DigiPen Institute of Technology Singapore. All rights reserved.
*****************************************************************************************/

#include "Precompiled.h"
#include "Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "Scene/SceneManager.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/MeshManager.h"
#include "Editor/EditorHeaders.h"

std::map<std::string, size_t> ComponentTypes{};

bool Transform::isLeaf() {
	size_t size = child.size();
	for (auto& c : child) {
		if (MySceneManager.GetCurrentScene().Get<Entity>(c).state == DELETED)
			size--;
	}
	return (size) ? false : true;
}

bool Transform::isChild() {
	if (parent != 0)
		return true;
	else
		return false;
}

glm::vec3 Transform::GetTranslation() const
{
	return glm::vec3(GetWorldMatrix()[3]);
}

glm::vec3 Transform::GetRotation() const
{
	//return glm::eulerAngles(glm::quat_cast(GetWorldMatrix()));
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	vec3 _scale;
	vec3 _translation;
	glm::mat4 globalTransform = GetWorldMatrix();
	// Calculate the global transformation matrix
	glm::decompose(globalTransform, _scale, rot, _translation, skew, perspective);
	return glm::eulerAngles(rot);
}

glm::vec3 Transform::GetScale() const
{
	if(parent)
		return MySceneManager.GetCurrentScene().Get<Transform>(parent).GetScale() * glm::vec3(scale);
	return scale;
}

glm::mat4 Transform::GetWorldMatrix() const
{
	if (parent)
		return MySceneManager.GetCurrentScene().Get<Transform>(parent).GetWorldMatrix() * GetLocalMatrix();
	return GetLocalMatrix();
}

glm::mat4 Transform::GetLocalMatrix() const {
	glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));

	return glm::translate(glm::mat4(1.0f), vec3(translation)) *
		rot *
		glm::scale(glm::mat4(1.0f), vec3(scale));
}

bool Transform::isEntityChild(Transform& ent) {
	if (std::find(child.begin(), child.end(), ent.EUID()) != child.end()) {
		return true;
	}
	if (!child.empty())
		return MySceneManager.GetCurrentScene().Get<Transform>(child.front()).isEntityChild(ent);
	return false;
}

void Transform::SetParent(Transform* newParent)
{
	if (newParent && newParent->EUID() == parent)
		return;
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	vec3 _scale;
	vec3 _translation;
	glm::mat4 globalTransform = GetWorldMatrix();
	// Calculate the global transformation matrix
	if (parent) {
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		parentTrans.RemoveChild(this);
		glm::decompose(globalTransform, _scale, rot, _translation, skew, perspective);
		scale = _scale;
		translation = _translation;
		rotation = glm::eulerAngles(rot);
	}

	if (!newParent)
		parent = 0;
	else
		parent = newParent->EUID();

	if (parent) {
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		glm::mat4 lTransform = glm::inverse(parentTrans.GetWorldMatrix()) * globalTransform;
		glm::decompose(lTransform, _scale, rot, _translation, skew, perspective);
		scale = _scale;
		translation = _translation;
		rotation = glm::eulerAngles(rot);
		// Add the object to the new parent's child list
		parentTrans.child.push_back(EUID());
	}
}

bool Transform::isSelectedChild() {
	if (!this->isLeaf()) {
		auto& children = this->child;

		for (auto& _child : children) {
			if (_child == EditorHierarchy::Instance().selectedEntity) {
				return true;
			}

			// Recursively check if the selectedEntity is a child or grandchild of this child.
			if (MySceneManager.GetCurrentScene().Get<Transform>(_child).isSelectedChild()) {
				return true; // Return true if found in the child or any of its descendants.
			}
		}
	}

	return false; 
}


void Transform::RemoveChild(Transform* t)
{
	auto it = std::find(child.begin(), child.end(), t->EUID());
	// Check if an element satisfying the condition was found
	E_ASSERT(it != child.end(), "FAILED TO REMOVE CHILD");
	// Erase the found element
	child.erase(it);
}

Animator::Animator() : BaseAnimator()
{
	playing = false;
}

Camera::Camera() : backgroundColor{ BaseCamera::backgroundColor }
{
	BaseCamera::Init();
	cameraType = CAMERATYPE::GAME;
}

void ParticleComponent::Initialize(int numParticles, float particleLifetime, float particleEmissionRate)
{
	numParticles_ = numParticles;
	particleLifetime_ = particleLifetime;
	particleEmissionRate_ = particleEmissionRate;
	//particles_.resize(numParticles_);
	// Initialize the position, velocity, and acceleration of each particle
	particles_.resize(numParticles_);
	for (int i = 0; i < numParticles_; i++) {
		particles_[i].position = glm::vec3(0.0f);
		particles_[i].velocity = glm::vec3(1.0f, 0.f, 0.f); // @kk changed this to test, pls do change back
		particles_[i].acceleration = glm::vec3(0.0f);
		particles_[i].lifetime = particleLifetime_;
	}
}
 

void ParticleComponent::Update(float dt)
{
	// Update the position, velocity, and acceleration of each particle
	for (int i = 0; i < numParticles_; i++) {
		particles_[i].position += particles_[i].velocity * dt;
		particles_[i].velocity += particles_[i].acceleration * dt;
		particles_[i].lifetime -= dt;
	}
	// Handle particle collisions
	// Emit new particles
	if (particleEmissionRate_ > 0.0f) {
		for (int i = 0; i < numParticles_; i++) {
			if (particles_[i].lifetime <= 0.0f) {
				particles_[i].position = glm::vec3(0.0f);
				//particles_[i].velocity = glm::vec3(0.0f); // shouldnt be 0
				particles_[i].acceleration = glm::vec3(0.0f); // shouldnt be 0?
				particles_[i].lifetime = particleLifetime_;
			}
		}
	}
}

void on_click_callback(void)
{
	printf("Button clicked!\n");
}

void ButtonComponent::Init()
{
	//Button* button = Button_init("button1",10, 10, 500, 500);
	//int mouse_x = 20, mouse_y = 20;
}

void ButtonComponent::Button_update(Button* button, int mouse_x, int mouse_y, bool left_mouse_button_clicked)
{
	if (button->x <= mouse_x && mouse_x <= button->x + button->width &&
		button->y <= mouse_y && mouse_y <= button->y + button->height) {
		if (left_mouse_button_clicked) {
			button->is_clicked = true;
			//button->on_click();
		}
	}
	else {
		button->is_clicked = false;
	}
}