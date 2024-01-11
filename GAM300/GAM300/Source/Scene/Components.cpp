﻿/*!***************************************************************************************
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
#include "Physics/PhysicsSystem.h"


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

void Transform::SetGlobalPosition(Vector3 newPos)
{
	//memcpy(&worldMatrix[3], &newPos, 3);
	glm::mat4 globalTransform = GetWorldMatrix();
	memcpy(&globalTransform[3], &newPos, sizeof(Vector3));
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	vec3 _scale;
	vec3 _translation;
	if (parent) 
	{
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		glm::mat4 lTransform = glm::inverse(parentTrans.GetWorldMatrix()) * globalTransform;
		glm::decompose(lTransform, _scale, rot, _translation, skew, perspective);
		//scale = _scale;
		translation = _translation;
		//rotation = glm::eulerAngles(rot);
	}
	else
	{
		translation = newPos;
	}
}

void Transform::SetGlobalRotation(Vector3 newRot)
{
	if (parent)
	{
		glm::quat rot;
		glm::vec3 skew;
		glm::vec4 perspective;
		vec3 _scale;
		vec3 _translation;
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		glm::mat4 lTransform = parentTrans.GetWorldMatrix();
		glm::decompose(lTransform, _scale, rot, _translation, skew, perspective);
		rotation = glm::eulerAngles(rot) - (glm::vec3)newRot;
		worldMatrix = lTransform * GetLocalMatrix();
	}
	else
	{
		rotation = newRot * 3.1415927;
		worldMatrix = GetLocalMatrix();
	}
}

void Transform::SetGlobalScale(Vector3 newScale)
{
	if (parent)
	{
		glm::quat rot;
		glm::vec3 skew;
		glm::vec4 perspective;
		vec3 _scale;
		vec3 _translation;
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		glm::mat4 lTransform = parentTrans.GetWorldMatrix();
		glm::decompose(lTransform, _scale, rot, _translation, skew, perspective);
		scale = (glm::vec3)newScale / _scale;
		worldMatrix = lTransform * GetLocalMatrix();
	}
	else
	{
		scale = newScale;
		worldMatrix = GetLocalMatrix();
	}
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
	return CreateTransformationMtx(translation, rotation, scale);
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

Camera::Camera() : backgroundColor{ BaseCamera::backgroundColor }
{
	BaseCamera::Init();
	cameraType = CAMERATYPE::GAME;
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