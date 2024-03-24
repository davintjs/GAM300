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
#include <glm/gtx/quaternion.hpp>
#include "Scene/SceneManager.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/MeshManager.h"
#include "Editor/EditorHeaders.h"
#include "Physics/PhysicsSystem.h"


namespace
{

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

void Transform::EnableFlag(Flag flag)
{
	flags |= static_cast<char>(flag);
	Scene& scene = MySceneManager.GetCurrentScene();
	for (const auto& childID : child)
	{
		Transform& tChild = scene.Get<Transform>(childID);
		//Child might still be false
		if (flag == Flag::WorldEnabled && scene.IsActive<Entity>(tChild,false) == false)
			continue;
		tChild.EnableFlag(flag);
	}
}

void Transform::DisableFlag(Flag flag)
{
	flags &= ~static_cast<char>(flag);
	Scene& scene = MySceneManager.GetCurrentScene();
	for (const auto& childID : child)
	{
		Transform& tChild = scene.Get<Transform>(childID);
		tChild.DisableFlag(flag);
	}
}

bool Transform::GetFlag(Flag flag)
{
	return (static_cast<char>(flag) & flags) != 0;
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

void Transform::UpdateEnabledFlags()
{
	
	if (parent != 0)
	{
		Scene& scene = MySceneManager.GetCurrentScene();
		Transform& p = scene.Get<Transform>(parent);
		if (scene.IsActive<Entity>(p))
			EnableFlag(Flag::WorldEnabled);
		else
			DisableFlag(Flag::WorldEnabled);
		for (auto& c : child)
		{
			scene.Get<Transform>(c).UpdateEnabledFlags();
		}
	}
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
	EnableFlag(Flag::Modified);
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
		glm::quat rotationQuat;
		Decompose(localMatrix, translation, rotationQuat, scale);
		rotation = glm::eulerAngles(rotationQuat);
	}
	else
	{
		translation = globalPos;
		rotation = globalRot;
		scale = globalScale;
	}
	EnableFlag(Flag::Modified);
	UpdateChildrenMatrices();
}

void Transform::UpdateWorldMatrix(glm::mat4& _world)
{
	glm::quat _rot;
	Decompose(_world, globalPos, _rot, globalScale);
	globalRot = glm::eulerAngles(_rot);
	worldMatrix = _world;
	EnableFlag(Flag::Modified);
	UpdateChildrenMatrices();
}

void Transform::UpdateChildrenMatrices()
{
	for (const auto& childID : child)
	{
		Transform& tChild = MySceneManager.GetCurrentScene().Get<Transform>(childID);
		glm::mat4 childMat = worldMatrix * tChild.GetLocalMatrix();
		tChild.UpdateWorldMatrix(childMat);
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

	// Calculate the global transformation matrix
	if (parent) {
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		parentTrans.RemoveChild(this);
	}

	if (!newParent)
	{
		parent = 0;
		scale = globalScale;
		translation = globalPos;
		rotation = globalRot;
		EnableFlag(Transform::Flag::WorldEnabled);
		//EnableFlag(Transform::Flag::Modified);
	}
	else
	{
		parent = newParent->EUID();
		Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(parent);
		glm::mat4 lTransform = glm::inverse(parentTrans.GetWorldMatrix()) * GetWorldMatrix();
		glm::quat tmpRot;
		Decompose(lTransform,translation,tmpRot,scale);
		rotation = glm::eulerAngles(tmpRot);
		parentTrans.child.push_back(EUID());
		bool worldEnabled = parentTrans.GetFlag(Transform::Flag::WorldEnabled);
		
		if (worldEnabled)
			EnableFlag(Transform::Flag::WorldEnabled);
		else
			DisableFlag(Transform::Flag::WorldEnabled);
			
		//EnableFlag(Transform::Flag::Modified);
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

Camera::Camera()
{
	BaseCamera::Init();
	cameraType = CAMERATYPE::GAME;
}

void on_click_callback(void)
{
	printf("Button clicked!\n");
}

//void ButtonComponent::Init()
//{
//	//Button* button = Button_init("button1",10, 10, 500, 500);
//	//int mouse_x = 20, mouse_y = 20;
//}

void ButtonComponent::Button_update(Button* button, int mouse_x, int mouse_y, bool left_mouse_button_clicked)
{
	//if (button->x <= mouse_x && mouse_x <= button->x + button->width &&
	//	button->y <= mouse_y && mouse_y <= button->y + button->height) {
	//	if (left_mouse_button_clicked) {
	//		button->is_clicked = true;
	//		printf("Button clicked!\n");
	//	}
	//}
	//else {
	//	button->is_clicked = false;
	//}

}