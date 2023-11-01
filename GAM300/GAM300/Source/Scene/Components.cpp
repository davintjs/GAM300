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

std::map<std::string, size_t> ComponentTypes{};

bool Transform::isLeaf() {
	return (child.size()) ? false : true;
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
	return glm::eulerAngles(glm::quat_cast(GetWorldMatrix()));
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

MeshFilter::MeshFilter()
{


	vertices = &MeshManager.GetMeshAsset(meshId).vertices;
	indices = &MeshManager.GetMeshAsset(meshId).indices;
}