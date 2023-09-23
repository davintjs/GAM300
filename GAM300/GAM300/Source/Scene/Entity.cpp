#include "Precompiled.h"
#include "Entity.h"
#include "SceneManager.h"

void Break_ParentChild(const ObjectIndex& _child) {
	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
	Transform& child = curr_scene.Get<Transform>(curr_scene.GetArray<Entity>().DenseSubscript(_child));

	//remove child from old parent
	auto& children = child.parent->child;
	auto it = std::find(children.begin(), children.end(), &child);
	children.erase(it);

	//set child's parent to nullptr now
	child.SetParent(nullptr);

}

void Set_ParentChild(Engine::UUID _parent, Engine::UUID _child) {
	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
	Transform& parent = curr_scene.Get<Transform>(curr_scene.GetArray<Entity>().DenseSubscript(_parent));
	Transform& child = curr_scene.Get<Transform>(curr_scene.GetArray<Entity>().DenseSubscript(_child));

	//if child has a previous parent, replace parent with this new one, remove child from old parent
	if (child.isChild()) {
		auto& children = child.parent->child;
		auto it = std::find(children.begin(), children.end(), &child);
		children.erase(it);
	}

	//set child's parent to new parent
	child.SetParent(&parent);
}

void Set_ParentChild(Transform& parent, Transform& child) {
	parent.child.push_back(&child);

	child.parent = &parent;
}


