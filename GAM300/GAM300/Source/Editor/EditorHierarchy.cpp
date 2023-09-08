#include "Precompiled.h"
#include "EditorHeaders.h"
#include "Editor.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

void EditorHierarchy::Init() {
    //no selected entity at start
    selectedEntity = NON_VALID_ENTITY;
}

//void EditorHierarchy::DisplayChildren(const ObjectIndex& parent) {
//    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
//        ImGuiTreeNodeFlags_OpenOnDoubleClick |
//        ImGuiTreeNodeFlags_SpanAvailWidth |
//        ImGuiTreeNodeFlags_DefaultOpen;
//
//    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();
//    Transform& currEntity = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(Index));
//
//    if (currEntity.isLeaf()) {
//        NodeFlags |= ImGuiTreeNodeFlags_Leaf;
//    }
//}


void EditorHierarchy::DisplayEntity(const ObjectIndex& Index) {

    // ImGuiTreeNodeFlags_SpanAvailWidth

    ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick |
        ImGuiTreeNodeFlags_DefaultOpen;

    if (Index == selectedEntity) {
        NodeFlags |= ImGuiTreeNodeFlags_Selected;
    }

    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

    Transform& currEntity = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(Index));

    if (currEntity.isLeaf()) {
        NodeFlags |= ImGuiTreeNodeFlags_Leaf;
    }

    auto EntityName = curr_scene.GetComponent<Tag>(curr_scene.entities.DenseSubscript(Index)).name.c_str();
    bool open = ImGui::TreeNodeEx(EntityName, NodeFlags);

    //select entity from hierarchy
    if (ImGui::IsItemClicked()) {
        selectedEntity = Index;
    }

    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("Entity", &selectedEntity, sizeof(selectedEntity));
        ImGui::Text(curr_scene.GetComponent<Tag>(curr_scene.entities.DenseSubscript(selectedEntity)).name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
            const ObjectIndex childId = *static_cast<ObjectIndex*>(payload->Data);
            
            Transform& currEntity = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(childId));
            Transform& targetEntity = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(Index));

            if (currEntity.isLeaf()) {
                if (childId != Index) {
                    Set_ParentChild(Index, childId);
                }
            }
            else {
                if (!currEntity.isEntityChild(targetEntity)) {
                    Set_ParentChild(Index, childId);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (open) {
        for (int i = 0; i < currEntity.child.size(); ++i) {
            ObjectIndex childId = curr_scene.singleComponentsArrays.GetArray<Transform>().GetDenseIndex(*currEntity.child[i]);
            DisplayEntity(childId);
        }   
        ImGui::TreePop();
    }
}

void EditorHierarchy::Update(float dt) {
    ImGui::Begin("Hierarchy");

    //List out all entities in current scene
    //When clicked on, shows all children
    //Drag and drop of entities into and from other entities to form groups (using a node system, parent child relationship)
    //Add/Delete entities using right click
    Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

   /* if (initLayer) {
        for (int i = 0; i < curr_scene.entities.size(); ++i) {
            layer.push_back(&curr_scene.entities[i]);
        }
        initLayer = false;
    }*/

    bool sceneopen = ImGui::TreeNodeEx(curr_scene.Scene_name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

    if(sceneopen){

        

        for (int i = 0; i < curr_scene.entities.size(); ++i) {


            /*std::string tag = "Entity " + std::to_string(i);
            ImGui::InvisibleButton(tag.c_str(), ImVec2(156, 1));
            if (ImGui::BeginDragDropTarget()) {
                const ImGuiPayload* payload = ImGui::GetDragDropPayload();
                const ObjectIndex Index = *static_cast<ObjectIndex*>(payload->Data);
                auto it = std::find(layer.begin(), layer.end(), &curr_scene.entities.DenseSubscript(Index));
                layer.erase(it);
                layer.insert(layer.begin() + i, &curr_scene.entities.DenseSubscript(Index));
                ImGui::EndDragDropTarget();
            }*/

            if (!curr_scene.GetComponent<Transform>(curr_scene.entities[i]).isChild()) {
                DisplayEntity(curr_scene.entities[i].denseIndex);
            }
        }

        if (ImGui::BeginDragDropTarget()) {
            /*if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {

                const ObjectIndex Index = *static_cast<ObjectIndex*>(payload->Data);

                Break_ParentChild(Index);
            }*/
            const ImGuiPayload* payload = ImGui::GetDragDropPayload();
            const ObjectIndex Index = *static_cast<ObjectIndex*>(payload->Data);

            if (curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(Index)).isChild()) {
                Break_ParentChild(Index);
            }

            ImGui::EndDragDropTarget();
        }

        //ImGui::InvisibleButton("##", ImVec2(156, 1));

        //Right click adding of entities in hierarchy window
        if (ImGui::BeginPopupContextWindow(0, true)) {
            if (ImGui::MenuItem("Add Entity")) {
                curr_scene.AddEntity();
            }

            if (ImGui::MenuItem("Delete Entity")) {
                Entity& ent = curr_scene.entities.DenseSubscript(selectedEntity);
                //Delete all children of selected entity as well
                auto currEntity = curr_scene.GetComponent<Transform>(curr_scene.entities.DenseSubscript(selectedEntity));
                for (auto child : currEntity.child) {
                    ObjectIndex id = curr_scene.singleComponentsArrays.GetArray<Transform>().GetDenseIndex(*child);
                    curr_scene.Destroy(child);
                }
                curr_scene.Destroy(ent);
            }
            ImGui::EndPopup();
        }

        ImGui::TreePop();
    }     
    ImGui::End();
}

void EditorHierarchy::Exit() {

}