/*!***************************************************************************************
\file			EditorScene.cpp
\project		
\author			Sean Ngo
\Coauthor       Joseph Ho

\par			Course : GAM300
\date           04/09/2023

\brief
	This file contains the definitions of the following:
	1. Editor Scene
    2. Editor Game

All content © 2023 DigiPen Institute of Technology Singapore.All rights reserved.
* *****************************************************************************************/
#include "Precompiled.h"
#include "EditorHeaders.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "IOManager/InputHandler.h"
#include "Editor.h"
#include "Editor/EditorCamera.h"
#include "ImGuizmo.h"
#include "Scene/SceneManager.h"
#include "Core/EventsManager.h"
#include "Graphics/MESHMANAGER.h"
#include "Graphics/GraphicsHeaders.h"

Transform oldTransform;
MeshRenderer oldMeshrenderer;

namespace
{
    const char* GizmoWorld[] = { "Local", "Global" };
    int GizmoType = ImGuizmo::TRANSLATE;
}

void EditorScene::Init()
{
    sceneDimension = glm::vec2(1600.f, 900.f);
    scenePosition = glm::vec2(0.f, 0.f);

    EVENTS.Subscribe(this, &EditorScene::CallbackEditorWindow);
}

void EditorScene::Update(float dt)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

    SceneView();

    ImGui::PopStyleVar();

    inOperation = ImGuizmo::IsOver() && EditorHierarchy::Instance().selectedEntity != NON_VALID_ENTITY;

    //update multiselect entities
    if (multiselectEntities.size()) {

        if (ImGui::IsKeyReleased(ImGuiKey_Escape))
            ClearMultiselect();
        
        Scene& curr_scene = MySceneManager.GetCurrentScene();
         useMeshRenderer = true;

        for (auto& ent : multiselectEntities) {
            if (!curr_scene.Has<MeshRenderer>(curr_scene.Get<Entity>(ent)))
                useMeshRenderer = false;
        }

        if (multiTransform != oldTransform) {
            for (auto& ent : multiselectEntities) {

                Transform& t = curr_scene.Get<Transform>(ent);
                t.SetGlobalPosition(t.GetGlobalTranslation() + (multiTransform.GetGlobalTranslation() - oldTransform.GetGlobalTranslation()));
                t.SetGlobalRotation(t.GetGlobalRotation() + multiTransform.GetGlobalRotation() - oldTransform.GetGlobalRotation());
                t.SetGlobalScale(t.GetGlobalScale() + multiTransform.GetGlobalScale() - oldTransform.GetGlobalScale());
                //t.translation += (multiTransform.translation - oldTransform.translation);
                //t.rotation += multiTransform.rotation - oldTransform.rotation;
                //t.scale += multiTransform.scale - oldTransform.scale;
            }
        }

        if (useMeshRenderer) {
            if (multiMeshRenderer != oldMeshrenderer) {
                for (auto& ent : multiselectEntities) {
                    MeshRenderer& Mesh = curr_scene.Get<MeshRenderer>(ent);
                    if (multiMeshRenderer.meshID != oldMeshrenderer.meshID) {
                        Mesh.meshID = multiMeshRenderer.meshID;
                    }
                    if (multiMeshRenderer.materialGUID != oldMeshrenderer.materialGUID) {
                        Mesh.materialGUID = multiMeshRenderer.materialGUID;
                    }
                }
            }
        }    
    }

    oldTransform = multiTransform;
}

// Calculate the center of selected objects
glm::vec3 CalculateCenterOfSelectedObjects() {
    glm::vec3 center(0.0f, 0.0f, 0.0f);

    auto& entities = EditorScene::Instance().multiselectEntities;
    for (const auto& entity : entities) {
        // Assuming each entity has a position component
        Scene& curr_scene = MySceneManager.GetCurrentScene();        
        center += curr_scene.Get<Transform>(entity).GetGlobalTranslation();
    }

    // Divide by the number of selected objects to get the average position
    center /= static_cast<float>(entities.size());
    return center;
}

glm::mat4 GetMultiselectWorldMatrix() {
    Transform& multiTransform = EditorScene::Instance().multiTransform;
    glm::mat4 rot = glm::toMat4(glm::quat(vec3(multiTransform.GetGlobalRotation())));

    return glm::translate(glm::mat4(1.0f), vec3(CalculateCenterOfSelectedObjects())) *
        rot *
        glm::scale(glm::mat4(1.0f), vec3(multiTransform.GetGlobalScale()));
}

void EditorScene::ToolBar()
{
    //Scene toolbar
    if (ImGui::BeginMenuBar())
    {
        ImGui::SetNextItemWidth(68.f);
        ImGui::Combo("##Coord Space", &coord_selection, GizmoWorld, 2, 2);

        // Separator
        ImGui::Dummy(ImVec2(20.f, 0.f));

        float buttonSize = 20.f;
        ImVec2 btn = ImVec2(buttonSize, buttonSize);

        static int toggled = 2;
        ImVec4 untoggledColor = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // Dark grey
        ImVec4 toggledColor = ImVec4(0.0f, 0.447f, 0.898f, 1.0f);
        ImVec4 buttonColor = untoggledColor;

        bool shouldPan = false; // Bean: This is for panning the camera using the Q key
        buttonColor = (toggled == 1) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        bool condition = !EditorCam.IsFlying() && (ImGui::IsKeyPressed(ImGuiKey_Q) && windowHovered);
        if (ImGui::Button("Q", btn) || condition)
        {
            toggled = 1;
        }

        // Pan the editor camera
        shouldPan = (toggled == 1) ? true : false;
        EditorPanCameraEvent e(shouldPan);
        EVENTS.Publish(&e);

        ImGui::PopStyleColor();

        buttonColor = (toggled == 2) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        if (ImGui::Button("W", btn) || (ImGui::IsKeyPressed(ImGuiKey_W) && windowHovered))
        {
            GizmoType = ImGuizmo::TRANSLATE;
            toggled = 2;
        }
        ImGui::PopStyleColor();

        buttonColor = (toggled == 3) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        condition = !EditorCam.IsFlying() && (ImGui::IsKeyPressed(ImGuiKey_E) && windowHovered);
        if (ImGui::Button("E", btn) || condition)
        {
            GizmoType = ImGuizmo::ROTATE;
            toggled = 3;
        }
        ImGui::PopStyleColor();

        buttonColor = (toggled == 4) ? toggledColor : untoggledColor;
        ImGui::PushStyleColor(ImGuiCol_Button, buttonColor); // Apply the button color
        if (ImGui::Button("R", btn) || (ImGui::IsKeyPressed(ImGuiKey_R) && windowHovered))
        {
            GizmoType = ImGuizmo::SCALE;
            toggled = 4;
        }
        ImGui::PopStyleColor();

        // Separator
        ImGui::Dummy(ImVec2(20.f, 0.f));

        //For thoe to change to toggle debug drawing
        if (ImGui::Checkbox("DD", &DEBUGDRAW.IsEnabled())) {}
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Show Debug Drawing (Camera Frustum, Colliders, etc)");
        }

        //if (ImGui::Checkbox("Render Shadows", &RENDERER.enableShadows())) {}

        ImGui::Dummy(ImVec2(10.f, 0.f));
        if (ImGui::Checkbox("FC", &RENDERER.EnableFrustumCulling())) {}
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Enable Frustum Culling (Cull away GameObjects that are not within the camera)");
        }

        ImGui::Dummy(ImVec2(10.f, 0.f));
        if (ImGui::Checkbox("SAC", &DEBUGDRAW.ShowAllColliders())) {}
        if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenDisabled))
        {
            ImGui::SetTooltip("Show All Colliders (Show all debug drawing of colliders)");
        }

        ImGui::EndMenuBar();
    }
}

void EditorScene::SceneView()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_MenuBar;
    windowOpened = ImGui::Begin("Scene", nullptr, flags);
    //Editor scene viewport
    if (windowOpened)
    {
        ToolBar();

        windowHovered = ImGui::IsWindowHovered();
        windowFocused = ImGui::IsWindowFocused();
        ImRect sceneRect = ImGui::GetCurrentWindow()->InnerRect;
        scenePosition = glm::vec2(sceneRect.Min.x, sceneRect.Min.y);
        unsigned int textureID = FRAMEBUFFER.GetTextureID(EditorCam.GetFramebufferID(), EditorCam.GetAttachment());
        ImVec2 viewportEditorSize = sceneRect.GetSize();
        glm::vec2 _newDimension = *((glm::vec2*)&viewportEditorSize);

        // Only if the current scene dimension is not the same as new dimension
        if (sceneDimension != _newDimension && _newDimension.x != 0 && _newDimension.y != 0)
        {
            sceneDimension = { _newDimension.x, _newDimension.y };
            EditorUpdateSceneGeometryEvent e(scenePosition, sceneDimension);
            EVENTS.Publish(&e);
            EditorCam.OnResize(sceneDimension.x, sceneDimension.y);
        }

        ImGui::Image((void*)(size_t)textureID, ImVec2{ (float)sceneDimension.x, (float)sceneDimension.y }, ImVec2{ 0 , 1 }, ImVec2{ 1 , 0 });

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {

                ContentBrowserPayload* data = static_cast<ContentBrowserPayload*>(payload->Data);
                Engine::HexID guid = data->guid;

                Scene& curr_scene = MySceneManager.GetCurrentScene();
                Scene::Layer& layer = curr_scene.layer;

                if (data->type == MODELTYPE) {
                    //Create new entity
                    Entity* ent = curr_scene.Add<Entity>();
                    Transform& parent = curr_scene.Get<Transform>(*ent);
                    curr_scene.Get<Tag>(*ent).name = data->name;

                    free(data->name);

                    //Get model 
                    GetAssetByGUIDEvent<ModelAsset> e{ data->guid };
                    EVENTS.Publish(&e);

                    auto model = e.importer;

                    //create each submesh of the model
                    for (auto& mesh : model->meshes) {
                        Entity* new_ent = curr_scene.Add<Entity>();
                        curr_scene.Add<MeshRenderer>(*new_ent);
                        //Attach dragged mesh GUID from the content browser
                        curr_scene.Get<MeshRenderer>(*new_ent).meshID = Engine::GUID<MeshAsset>(mesh);

                        Transform& child = curr_scene.Get<Transform>(*new_ent);
                        curr_scene.Add<Tag>(*new_ent);
                        Tag& tag = curr_scene.Get<Tag>(*new_ent);
                        GetAssetByGUIDEvent<MeshAsset> _mesh { mesh };
                        EVENTS.Publish(&_mesh);
                        tag.name = _mesh.asset->mFilePath.stem().string().c_str();

                        //set the model submeshes to the parent group
                        child.SetParent(&parent);
                    }
                }
                else if (data->type == MATERIAL) {
                    //check which entity the mouse is current at when item is dropped
                    Engine::UUID temp = COLOURPICKER.ColorPickingMeshs(EditorCam);

                    //if valid entity
                    if (temp != 0){
                        Entity& currEntity = curr_scene.Get<Entity>(temp);

                        //if object does not have a mesh renderer
                        if (!curr_scene.Has<MeshRenderer>(currEntity))
                            curr_scene.Add<MeshRenderer>(currEntity);

                        //Assign material to mesh renderer
                        MeshRenderer& meshrenderer = curr_scene.Get<MeshRenderer>(currEntity);
                        Change newchange(&meshrenderer, "MeshRenderer/Material_ID");
                        Engine::GUID<MaterialAsset> matGuid = guid;
                        EDITOR.History.SetPropertyValue(newchange, curr_scene.Get<MeshRenderer>(currEntity).materialGUID, matGuid);
                    }
                }

               
                //add other file types here              
            }
            ImGui::EndDragDropTarget();
        }

        // Display the gizmos for the selected entity
        DisplayGizmos();
    }
    ImGui::End();
}

bool EditorScene::SelectEntity()
{
    if (!inOperation && !EditorCam.IsMoving() && InputHandler::isMouseButtonPressed_L())
    {
        // Bean: Click within the scene imgui window
        if (!windowHovered)
            return false;

        EditorCam.GetRay() = EditorCam.Raycasting();
        if (EditorCam.GetIntersect() == FLT_MAX)
        {
            // This means that u double clicked, wanted to select something, but THERE ISNT ANYTHING
            SelectedEntityEvent selectedEvent{ 0 };
            EVENTS.Publish(&selectedEvent);
        }
        return true;
    }

    return false;
}

void EditorScene::ClearMultiselect() {
    multiselectEntities.clear();
    multiTransform = Transform();
    multiMeshRenderer = MeshRenderer();
}

void EditorScene::DisplayGizmos()
{
    Scene& currentScene = MySceneManager.GetCurrentScene();

    if (SelectEntity())
    {
        // Colour picking Version if needed
        /*Engine::UUID temp = COLOURPICKER.ColorPickingMeshs(EditorCam);
        if (temp == 0)
        {
            SelectedEntityEvent SelectingEntity(0);
            EVENTS.Publish(&SelectingEntity);
        }
        else
        {
            Entity& currEntity = currentScene.Get<Entity>(temp);

            SelectedEntityEvent SelectingEntity(&currEntity);
            EVENTS.Publish(&SelectingEntity);
        }*/
        float& intersect = EditorCam.GetIntersect();
        float& tempIntersect = EditorCam.GetTempIntersect();

        for (MeshRenderer& renderer : currentScene.GetArray<MeshRenderer>())
        {
            if (renderer.state == DELETED) continue;

            if (!currentScene.IsActive(renderer)) continue;

            Entity& entity = currentScene.Get<Entity>(renderer);
            if (!currentScene.IsActive(entity)) continue;

            Transform& transform = currentScene.Get<Transform>(entity);
            
            glm::mat4 transMatrix = transform.GetWorldMatrix();

            glm::vec3 translation;
            glm::vec3 rot;
            glm::vec3 scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transMatrix), &translation[0], &rot[0], &scale[0]);
            glm::vec3 mins = scale, maxs = scale;

            Mesh* mesh = MESHMANAGER.DereferencingMesh(renderer.meshID);
            if (mesh)
            {
                mins *= mesh->vertices_min;
                maxs *= mesh->vertices_max;
            }
            else
            {
                Tag& tag = currentScene.Get<Tag>(entity);
                std::string error;
                error = "ERROR: " + tag.name + " has guid " + renderer.meshID.ToHexString() + " that does not exist! Reassign the mesh!\n";
                E_ASSERT(false, error);
            }
            
            rot = glm::radians(rot);
            glm::mat4 rotMat = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

            Ray3D temp = EditorCam.GetRay();
            if (temp.TestRayOBB(glm::translate(glm::mat4(1.0f), translation) * rotMat, mins, maxs, tempIntersect))
            {
                if (tempIntersect < intersect)
                {
                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                        if (std::find(multiselectEntities.begin(), multiselectEntities.end(), renderer.EUID()) == multiselectEntities.end()) {
                            multiselectEntities.push_back(renderer.EUID());
                        } 
                        else {
                            ClearMultiselect();
                        }

                        GetSelectedEntityEvent e{};
                        EVENTS.Publish(&e);         
                        if(e.pEntity != nullptr)
                            multiselectEntities.push_back(e.pEntity->EUID());
                    }
                    SelectedEntityEvent SelectingEntity(&entity);
                    EVENTS.Publish(&SelectingEntity);
                    intersect = tempIntersect;
                    EditorHierarchy::Instance().newselect = true;
                    EditorHierarchy::Instance().movetoitem = true;
                }
            }
        }

        for (SpriteRenderer& Sprite : currentScene.GetArray<SpriteRenderer>())
        {
            if (Sprite.state == DELETED) continue;
            Entity& entity = currentScene.Get<Entity>(Sprite);
            if (!currentScene.IsActive(entity)) continue;
            Transform& transform = currentScene.Get<Transform>(entity);

            // I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
            glm::mat4 transMatrix = transform.GetWorldMatrix();

            glm::vec3 translation;
            glm::vec3 rot;
            glm::vec3 scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transMatrix), &translation[0], &rot[0], &scale[0]);

            //glm::vec3 mins = scale * MESHMANAGER.DereferencingMesh(renderer.MeshName)->vertices_min;
            //glm::vec3 maxs = scale * MESHMANAGER.DereferencingMesh(renderer.MeshName)->vertices_max;
            glm::vec3 mins = -scale;
            glm::vec3 maxs = scale;
            rot = glm::radians(rot);
            glm::mat4 rotMat = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

            Ray3D temp = EditorCam.GetRay();
            if (temp.TestRayOBB(glm::translate(glm::mat4(1.0f), translation) * rotMat, mins, maxs, tempIntersect))
            {
                if (tempIntersect < intersect)
                {
                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                        if (std::find(multiselectEntities.begin(), multiselectEntities.end(), Sprite.EUID()) == multiselectEntities.end()) {
                            multiselectEntities.push_back(Sprite.EUID());
                        }
                        
                    }
                    SelectedEntityEvent SelectingEntity(&entity);
                    EVENTS.Publish(&SelectingEntity);
                    intersect = tempIntersect;
                    EditorHierarchy::Instance().newselect = true;
                    EditorHierarchy::Instance().movetoitem = true;
                }
            }
        }

        for (BoxCollider& bc : currentScene.GetArray<BoxCollider>())
        {
            Entity& entity = currentScene.Get<Entity>(bc);
            if (!currentScene.IsActive(entity)) continue;

            if (currentScene.Has<MeshRenderer>(entity)) continue;

            if (bc.state == DELETED) continue;

            Transform& transform = currentScene.Get<Transform>(entity);
            Tag& tag = currentScene.Get<Tag>(entity);

            glm::mat4 transMatrix;
            // I am putting it here temporarily, maybe this should move to some editor area :MOUSE PICKING
            transMatrix = transform.GetWorldMatrix();

            glm::vec3 translation;
            glm::vec3 rot;
            glm::vec3 scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transMatrix), &translation[0], &rot[0], &scale[0]);
            glm::vec3 mins = scale * MESHMANAGER.DereferencingMesh(ASSET_CUBE)->vertices_min;
            glm::vec3 maxs = scale * MESHMANAGER.DereferencingMesh(ASSET_CUBE)->vertices_max;
            mins *= glm::vec3(bc.dimensions.x, bc.dimensions.y, bc.dimensions.z);
            maxs *= glm::vec3(bc.dimensions.x, bc.dimensions.y, bc.dimensions.z);
            rot = glm::radians(rot);
            glm::mat4 rotMat = glm::eulerAngleXYZ(rot.x, rot.y, rot.z);

            Ray3D temp = EditorCam.GetRay();
            if (temp.TestRayOBB(glm::translate(glm::mat4(1.0f), translation) * rotMat, mins, maxs, tempIntersect))
            {
                if (tempIntersect < intersect)
                {
                    if (ImGui::IsKeyDown(ImGuiKey_LeftShift)) {
                        if (std::find(multiselectEntities.begin(), multiselectEntities.end(), bc.EUID()) == multiselectEntities.end()) {
                            multiselectEntities.push_back(bc.EUID());
                        }
                    }
                    SelectedEntityEvent SelectingEntity(&entity);
                    EVENTS.Publish(&SelectingEntity);
                    intersect = tempIntersect;
                    EditorHierarchy::Instance().newselect = true;
                    EditorHierarchy::Instance().movetoitem = true;
                }
            }
        }
    }

    // Display gizmos & debug for selected entity
    if (EDITOR.GetSelectedEntity() != 0)
    {
        Entity& entity = currentScene.Get<Entity>(EDITOR.GetSelectedEntity());
        bool isActive = currentScene.IsActive(entity);
        Transform& trans = currentScene.Get<Transform>(entity);

        Vector3 tempScale = trans.GetLocalScale();
        for (int i = 0; i < 3; ++i)
        {
            if (fabs(tempScale[i]) < 0.001f)
                tempScale[i] = 0.001f;
        }
        trans.SetLocalScale(tempScale);

        // Drawing box collider of selected object
        if (currentScene.Has<BoxCollider>(entity) && isActive)
        {
            BoxCollider& bc = currentScene.Get<BoxCollider>(entity);
            RigidDebug currRigidDebug;
            currRigidDebug.vao = MESHMANAGER.offsetAndBoundContainer[ASSET_CUBE].vao;
            glm::mat4 SRT = trans.GetWorldMatrix();
            glm::mat4 scalarMat = glm::scale(glm::mat4(1.f), glm::vec3(bc.dimensions));
            glm::mat4 transMat = glm::translate(glm::mat4(1.f), glm::vec3(bc.offset));
            SRT *= transMat * scalarMat;
            currRigidDebug.SRT = SRT;
            DEBUGDRAW.AddBoxColliderDraw(currRigidDebug);
        }

        if (currentScene.Has<CapsuleCollider>(entity) && isActive)
        {
            //DEBUGDRAW.DrawCapsuleBounds(entity.EUID());
        }

        if (currentScene.Has<SphereCollider>(entity) && isActive)
        {
            DEBUGDRAW.DrawSphereBounds(entity.EUID());
        }

        if (currentScene.Has<Camera>(entity) && isActive)
        {
            DEBUGDRAW.DrawCameraBounds(entity.EUID());
        }

        if (currentScene.Has<LightSource>(entity) && isActive)
        {
            DEBUGDRAW.DrawLightBounds(entity.EUID());
        }

        if (currentScene.Has<SpriteRenderer>(entity) && isActive)
        {
            DEBUGDRAW.DrawButtonBounds(entity.EUID());
        }

        glm::mat4 transform_1;
        if (EditorScene::Instance().multiselectEntities.size() > 1)
            transform_1 = GetMultiselectWorldMatrix();
        else
            transform_1 = trans.GetWorldMatrix();

        glm::mat4 og_transform = transform_1;


        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(scenePosition.x, scenePosition.y, sceneDimension.x, sceneDimension.y);
        ImGuizmo::Manipulate(glm::value_ptr(EditorCam.GetViewMatrix()), glm::value_ptr(EditorCam.GetProjMatrix()),
            (ImGuizmo::OPERATION)GizmoType, (ImGuizmo::MODE)coord_selection, glm::value_ptr(transform_1));

        static bool firstmove = true;
        static Transform origTransform;

        if (ImGuizmo::IsUsing())
        {
            if (firstmove) {
                origTransform = trans;
                firstmove = false;
            }

            if (trans.parent)
            {
                Transform& parentTrans = MySceneManager.GetCurrentScene().Get<Transform>(trans.parent);
                glm::mat4 parentTransform = parentTrans.GetWorldMatrix();
                transform_1 = glm::inverse(parentTransform) * transform_1;
            }

            glm::vec3 a_translation;
            glm::vec3 a_rot;
            glm::vec3 a_scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(transform_1), &a_translation[0], &a_rot[0], &a_scale[0]);
            //trans.SetLocalMatrix(a_translation, a_rot, a_scale);

            //old values to calculate offset
            glm::vec3 o_translation;
            glm::vec3 o_rot;
            glm::vec3 o_scale;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(og_transform), &o_translation[0], &o_rot[0], &o_scale[0]);

            if (multiselectEntities.size()) {

                glm::vec3 translation = multiTransform.GetLocalTranslation() + (a_translation - o_translation);
                glm::vec3 rotation = multiTransform.GetLocalRotation() + (glm::radians(a_rot - o_rot));
                auto offset = a_scale - o_scale;
                offset /= 200; //hardcoded value for now
                glm::vec3 scale = multiTransform.GetLocalScale() + offset;
                multiTransform.SetLocalMatrix(translation, rotation, scale);
                //multiTransform.translation += (a_translation - o_translation);
                //multiTransform.rotation += (glm::radians(a_rot - o_rot));
                //multiTransform.scale += offset;
            }
            else{
                trans.SetLocalMatrix(a_translation, glm::radians(a_rot), a_scale);
                /*trans.translation = a_translation;
                trans.rotation = glm::radians(a_rot);
                trans.scale = a_scale;*/
            }
            
        }
        else if (!firstmove) {
            if (trans.GetGlobalTranslation() != origTransform.GetGlobalTranslation()) {
                Change translate(&trans, "Transform/Translation");
                //EDITOR.History.SetPropertyValue(translate, origTransform.translation, trans.translation);
                glm::vec3 origTrans = origTransform.GetGlobalTranslation();
                glm::vec3 transTrans = trans.GetGlobalTranslation();
                EDITOR.History.SetPropertyValue(translate, origTrans, transTrans);
                origTransform.SetGlobalPosition(origTrans);
                trans.SetGlobalPosition(transTrans);
            }
            if (trans.GetGlobalRotation() != origTransform.GetGlobalRotation()) {
                Change rotate(&trans, "Transform/Rotation");
                //EDITOR.History.SetPropertyValue(rotate, origTransform.rotation, trans.rotation);
                glm::vec3 origRot = origTransform.GetGlobalRotation();
                glm::vec3 transRot = trans.GetGlobalRotation();
                EDITOR.History.SetPropertyValue(rotate, origRot, transRot);
                origTransform.SetGlobalRotation(origRot);
                trans.SetGlobalRotation(transRot);
            }
            if (trans.GetGlobalScale() != origTransform.GetGlobalScale()) {
                Change scale(&trans, "Transform/Scale");
                //EDITOR.History.SetPropertyValue(scale, origTransform.scale, trans.scale);
                glm::vec3 origScale = origTransform.GetGlobalScale();
                glm::vec3 transScale = trans.GetGlobalScale();
                EDITOR.History.SetPropertyValue(scale, origScale, transScale);
                origTransform.SetGlobalScale(origScale);
                trans.SetGlobalScale(transScale);
            }
            firstmove = true;
        }
    }
    else {
        ClearMultiselect();
    }
        

}

void EditorScene::Exit()
{

}

void EditorScene::CallbackEditorWindow(EditorWindowEvent* pEvent)
{
    // If not scene window
    if (pEvent->name.compare("Scene"))
        return;

    pEvent->isOpened = WindowOpened();
    pEvent->isHovered = WindowHovered();
    pEvent->isFocused = WindowFocused();
}