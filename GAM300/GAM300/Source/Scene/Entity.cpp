#include "Precompiled.h"
#include "Entity.h"
#include "SceneManager.h"

void Break_ParentChild(const ObjectIndex& _child) {
	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
	Transform& child = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(_child));

	child.SetParent(nullptr);
	//remove child from old parent
	//auto& children = child.parent->child;
	//auto it = std::find(children.begin(), children.end(), &child);
	//children.erase(it);

	////child does not have a parent now, is a leaf node
	//child.parent = nullptr;
}

void Set_ParentChild(const ObjectIndex& _parent, const ObjectIndex& _child) {
	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
	Transform& parent = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(_parent));
	Transform& child = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(_child));
	child.SetParent(&parent);

	//if child has a previous parent, replace parent with this new one, remove child from old parent
	//if (child.isChild()) {
	//	auto& children = child.parent->child;
	//	auto it = std::find(children.begin(), children.end(), &child);
	//	children.erase(it);
	//}

	//parent.child.push_back(&child);

	//child.parent = &parent;
}

void Set_ParentChild(Transform& parent, Transform& child) {
	parent.child.push_back(&child);

	child.parent = &parent;
}


