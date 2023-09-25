#include "Precompiled.h"
#include "Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
//
//
//template <>
//const char* GetTypeGroup<>::name = "";
bool Transform::isLeaf() {
	return (child.size()) ? false : true;
}

bool Transform::isChild() {
	if (parent != nullptr)
		return true;
	else
		return false;
}

glm::mat4 Transform::GetWorldMatrix() const
{
	if (parent)
		return parent->GetWorldMatrix() * GetLocalMatrix();
	return GetLocalMatrix();
}

glm::mat4 Transform::GetLocalMatrix() const {
	glm::mat4 rot = glm::toMat4(glm::quat(vec3(rotation)));

	return glm::translate(glm::mat4(1.0f), vec3(translation)) *
		rot *
		glm::scale(glm::mat4(1.0f), vec3(scale));
}

bool Transform::isEntityChild(Transform& ent) {
	if (std::find(child.begin(), child.end(), &ent) != child.end()) {
		return true;
	}
	for (int i = 0; i < child.size(); i++) {
		return child[i]->isEntityChild(ent);
	}
	return false;
}

void Transform::SetParent(Transform* newParent)
{
	if (newParent == parent)
		return;
	glm::quat rot;
	glm::vec3 skew;
	glm::vec4 perspective;
	vec3 _scale;
	vec3 _translation;
	glm::mat4 globalTransform = GetWorldMatrix();
	// Calculate the global transformation matrix
	if (parent) {
		parent->RemoveChild(this);
		glm::decompose(globalTransform, _scale, rot, _translation, skew, perspective);
		scale = _scale;
		translation = _translation;
		rotation = glm::eulerAngles(rot);
	}

	parent = newParent;

	if (parent) {
		glm::mat4 lTransform = glm::inverse(parent->GetWorldMatrix()) * globalTransform;
		glm::decompose(lTransform, _scale, rot, _translation, skew, perspective);
		scale = _scale;
		translation = _translation;
		rotation = glm::eulerAngles(rot);
		// Add the object to the new parent's child list
		parent->child.push_back(this);
	}
}


void Transform::RemoveChild(Transform* t)
{
	auto it = std::find(child.begin(), child.end(), t);
	// Check if an element satisfying the condition was found
	E_ASSERT(it != child.end(), "FAILED TO REMOVE CHILD");
	// Erase the found element
	child.erase(it);
}


Transform::~Transform()
{
	SetParent(nullptr);
	for (Transform* transform : child)
	{
		transform->SetParent(parent);
	}
}