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
#include "Physics/PhysicsSystem.h"


namespace
{
	static void Decompose(glm::mat4& mat, Vector3& pos, glm::quat& rot, Vector3& scale)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(mat, (glm::vec3&)scale, rot, (glm::vec3&)pos, skew, perspective);
	}
}

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

void Transform::SetGlobalPosition(Vector3 newPos)
{
	SetWorldMatrix(newPos, globalRot, globalScale);
}

void Transform::SetGlobalRotation(Vector3 newRot)
{
	SetWorldMatrix(globalPos, newRot, globalScale);
}

void Transform::SetGlobalScale(Vector3 newScale)
{
	SetWorldMatrix(globalPos, globalRot, newScale);
}


void Transform::SetLocalPosition(Vector3 newPos)
{
	SetLocalMatrix(newPos, rotation, scale);
}

void Transform::SetLocalRotation(Vector3 newRot)
{
	SetLocalMatrix(translation, newRot, scale);
}

void Transform::SetLocalScale(Vector3 newScale)
{
	SetLocalMatrix(translation, rotation, newScale);
}

Transform* Transform::GetParent() 
{ 
	if (parent) 
		return &MySceneManager.GetCurrentScene().Get<Transform>(parent); 
	return nullptr;
}

void Transform::SetLocalMatrix(vec3 _translation, vec3 _rotation, vec3 _scale)
{
	translation = _translation;
	rotation = _rotation;
	scale = _scale;
	if (parent)
	{
		Transform* tParent = GetParent();
		worldMatrix = tParent->GetWorldMatrix() * CreateTransformationMtx(translation, rotation, scale);
		glm::quat worldRotQ;
		Decompose(worldMatrix, globalPos, worldRotQ, globalScale);
		globalRot = glm::eulerAngles(worldRotQ);
	}
	else
	{
		worldMatrix = CreateTransformationMtx(translation, rotation, scale);
		globalPos = translation;
		globalRot = rotation;
		globalScale = scale;
	}
	UpdateChildrenMatrices();
}

void Transform::SetWorldMatrix(vec3 _translation, vec3 _rotation, vec3 _scale)
{
	globalPos = _translation;
	globalRot = _rotation;
	globalScale = _scale;
	worldMatrix = CreateTransformationMtx(_translation, _rotation, _scale);
	if (parent)
	{
		Transform* tParent = GetParent();
		glm::mat4 localMatrix = glm::inverse(tParent->GetWorldMatrix())* worldMatrix;
		
		Decompose(localMatrix, translation, rotationQuat, scale);
		rotation = glm::eulerAngles(rotationQuat);
	}
	else
	{
		translation = globalPos;
		rotation = globalRot;
		scale = globalScale;
	}
	UpdateChildrenMatrices();
}


void Transform::UpdateChildrenMatrices()
{
	for (const auto& childID : child)
	{
		Transform& tChild = MySceneManager.GetCurrentScene().Get<Transform>(childID);
		glm::mat4 childMat = worldMatrix * tChild.GetLocalMatrix();
		Vector3 cPos;
		glm::quat cRot;
		Vector3 cScale;
		Decompose(childMat,cPos,cRot,cScale);
		tChild.SetWorldMatrix(cPos,glm::eulerAngles(cRot),cScale);
	}
}

glm::mat4 Transform::GetWorldMatrix() const
{
	return worldMatrix;
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

void Transform::TempSetLocal(Vector3 _pos, Vector3 _rot, Vector3 _scale)
{
	translation = _pos;
	rotation = _rot;
	scale = _scale;
}

void Transform::RecalculateLocalMatrices()
{
	SetLocalMatrix(translation,rotation,scale);
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