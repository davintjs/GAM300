#include "Precompiled.h"
#include "EditorHeaders.h"
#include "Editor.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"


void EditorHierarchy::Init() {

}

void EditorHierarchy::Update(float dt) {
    ImGui::Begin("Hierarchy");

    //List out all entities in current scene
    //When clicked on, shows all children
    //Drag and drop of entities into and from other entities to form groups (using a node system, parent child relationship)
    //Add/Delete entities using right click
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_Leaf;

    for (int i = 0; i < curr_scene.entities.size(); ++i) {       
        
        Transform& currEntity = curr_scene.singleComponentsArrays.GetArray<Transform>()[i];
        if (currEntity.isLeaf()) {
            NodeFlags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool open = ImGui::TreeNodeEx(curr_scene.singleComponentsArrays.GetArray<Tag>()[i].name.c_str());
        if (open) {
            ImGui::TreePop();
        }
        
    }
    
    ImGui::End();
}

void EditorHierarchy::Exit() {

}