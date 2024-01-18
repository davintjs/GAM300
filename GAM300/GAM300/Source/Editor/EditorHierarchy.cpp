/*!***************************************************************************************
\file			EditorHierarchy.cpp
\project
\author         Joseph Ho

\par			Course: GAM300
\date           07/09/2023

\brief
	This file contains the definitions of the functions that renders the Hierarchy window in
	Editor. These functionalities include:
	1. Displaying entities in a scene in a hierarchial tree format
	2. Parenting and reordering of entities in the tree
	3. Adding/removing of entities from the hierarchy window

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "EditorHeaders.h"
#include "Editor.h"
#include "Scene/Scene.h"
#include "Core/EventsManager.h"
#include "Scene/SceneManager.h"
#include "Utilities/Serializer.h"


void EditorHierarchy::Init()
{
	//no selected entity at start
	selectedEntity = NON_VALID_ENTITY;
	EVENTS.Subscribe(this, &EditorHierarchy::CallbackSelectedEntity);
}

//Display the entity in the hierarchy tree
void EditorHierarchy::DisplayEntity(Engine::UUID euid)
{

	// ImGuiTreeNodeFlags_SpanAvailWidth

	ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick;


	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

	Scene::Layer& layer = curr_scene.layer;
	Transform& currEntity = curr_scene.Get<Transform>(euid);

	if (currEntity.isLeaf())
	{
		NodeFlags |= ImGuiTreeNodeFlags_Bullet;
	}

	//Invisible button for drag drop reordering
	float button_width = (ImGui::GetWindowContentRegionWidth() > 0) ? ImGui::GetWindowContentRegionWidth() : 0.1f;
	ImGui::InvisibleButton("##", ImVec2(button_width, 2.5f));


	//Drag drop reordering implementation
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
		{
			Engine::UUID childId = *static_cast<Engine::UUID*>(payload->Data);


			if (childId != euid)
			{
				Transform& currTransform = curr_scene.Get<Transform>(childId);
				Transform& targetTransform = curr_scene.Get<Transform>(euid);

				Transform& currParent = curr_scene.Get<Transform>(currTransform.parent);
				Transform& targetParent = curr_scene.Get<Transform>(targetTransform.parent);

				//if target entity is a child
				if (targetTransform.isChild())
				{

					//if current entity has a parent
					if (currTransform.isChild())
					{

						//if reordering within the same parent
						if (currParent.child == targetParent.child)
						{

							auto& arr = targetParent.child;
							//delete curr entity from layer position
							//           
							auto prev_it = std::find(arr.begin(), arr.end(), currTransform.EUID());
							arr.erase(prev_it);
							//reorder (reinsert) the current entity into new layer position
							auto it = std::find(arr.begin(), arr.end(), targetTransform.EUID());
							arr.insert(it, childId);
						}
						//if current entity has a different previous parent, remove it.
						else
						{
							//auto& children = currParent.child;
							currTransform.SetParent(&targetParent);

							auto& arr = targetParent.child;
							//Reorder entity to target entity location     
							auto prev_it = std::find(arr.begin(), arr.end(), currTransform.EUID());
							arr.erase(prev_it);
							//reorder (reinsert) the current entity into new layer position
							auto it2 = std::find(arr.begin(), arr.end(), targetTransform.EUID());
							arr.insert(it2, currTransform.EUID());
						}
					}
					//if current entity is a base node (no parent)
					else
					{
						//delete instance of entity in container
						/*auto prev_it = std::find(layer.begin(), layer.end(), &currEntity);
						layer.erase(prev_it);*/

						auto& parent = targetParent.child;

						currTransform.SetParent(&targetParent);
						if (parent.size() > 0)
						{
							parent.pop_back();
							auto it = std::find(parent.begin(), parent.end(), targetTransform.EUID());
							parent.insert(it, currTransform.EUID());
						}
					}

				}
				//if drag entity out of a group
				else
				{
					//if current entity has a parent, delink it
					if (currTransform.isChild())
					{
						currTransform.SetParent(nullptr);
					}
					//delete instance of entity in container
					auto prev_it = std::find(layer.begin(), layer.end(), childId);
					layer.erase(prev_it);

					//reorder (reinsert) the current entity into new layer position
					auto it = std::find(layer.begin(), layer.end(), euid);
					layer.insert(it, childId);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	bool multiselect = false;
	std::list<Engine::UUID>& multiSel = EditorScene::Instance().multiselectEntities;
	if (std::find(multiSel.begin(), multiSel.end(), euid) != multiSel.end()) {
		multiselect = true;
	}

	if (currEntity.isSelectedChild() || (euid == selectedEntity) || multiselect)  {
		
		ImGui::SetNextItemOpen(true);

		if (newselect) {
			ImGui::SetNextItemOpen(true);
			newselect = false;
		}

		if (euid == selectedEntity) {
			if (movetoitem) {
				ImGui::SetScrollHereY();  // Scroll to the current item
				movetoitem = false;
			}
			NodeFlags |= ImGuiTreeNodeFlags_Selected;
		}		

		if (multiselect)
			NodeFlags |= ImGuiTreeNodeFlags_Selected;

	}
	 
	auto EntityName = curr_scene.Get<Tag>(euid).name.c_str();
	bool open = ImGui::TreeNodeEx(EntityName, NodeFlags);

	ImVec2 minBound = ImGui::GetItemRectMin();
	ImVec2 maxBound = ImGui::GetItemRectMax();

	minBound.x += 20; //offset the arrow button

	//if user is pressing on the entity hierarchy button rather than arrow button
	if (ImGui::IsMouseReleased(0) && ImGui::IsMouseHoveringRect(minBound, maxBound)) {
		SelectedEntityEvent selectedEvent{ &curr_scene.Get<Entity>(euid) };
		EVENTS.Publish(&selectedEvent);
	}

	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("Entity", &selectedEntity, sizeof(Handle));
		ImGui::Text(curr_scene.Get<Tag>(selectedEntity).name.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
		{
			Engine::UUID childId = *static_cast<Engine::UUID*>(payload->Data);

			Transform& currT = curr_scene.Get<Transform>(childId);
			Transform& targetEntity = curr_scene.Get<Transform>(euid);

			if (currT.isLeaf())
			{
				if (childId != euid)
				{
					currT.SetParent(&targetEntity);
				}
			}
			else
			{
				if (!currT.isEntityChild(targetEntity))
				{
					currT.SetParent(&targetEntity);
				}
			}
		}
		ImGui::EndDragDropTarget();
	}

	if (open)
	{
		for (int i = 0; i < currEntity.child.size(); ++i)
		{
			Engine::UUID childId = currEntity.child[i];
			//Display non-deleted child
			if (MySceneManager.GetCurrentScene().Get<Entity>(childId).state != DELETED)
				DisplayEntity(childId);
		}
		ImGui::TreePop();
	}
}

void EditorHierarchy::Update(float dt)
{
	UNREFERENCED_PARAMETER(dt);

	ImGui::Begin("Hierarchy");

	//List out all entities in current scene
	//When clicked on, shows all children
	//Drag and drop of entities into and from other entities to form groups (using a node system, parent child relationship)
	//Add/Delete entities using right click
	static ImGuiTextFilter filter;
	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

	ImGui::Text("Filter: "); ImGui::SameLine();
	filter.Draw();

	ImGui::BeginChild("ScrollingRegion", ImVec2(0, -20.f), false);

	bool sceneopen = ImGui::TreeNodeEx(curr_scene.sceneName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	Scene::Layer& layer = curr_scene.layer;

	if (sceneopen)
	{
		for (Engine::UUID euid : layer)
		{
			if (!curr_scene.Get<Transform>(euid).isChild())
			{
				//check if filter characters matches the name and filter accordingly
				if (!filter.PassFilter(curr_scene.Get<Tag>(euid).name.c_str()))
					continue;

				if (curr_scene.Get<Entity>(euid).state == DELETED) continue;

				//Recursive function to display entities in a hierarchy tree
				DisplayEntity(euid);
			}
		}

		ImVec2 buttonsize;
		buttonsize.x = (ImGui::GetContentRegionAvail().x > 0) ? ImGui::GetContentRegionAvail().x : 0.01f;
		buttonsize.y = (ImGui::GetContentRegionAvail().y > 0) ? ImGui::GetContentRegionAvail().y : 0.01f;
		ImGui::InvisibleButton("##", buttonsize );

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity")) {
				Engine::UUID Index = *static_cast<Engine::UUID*>(payload->Data);

				Transform& currTrans = curr_scene.Get<Transform>(Index);

				if (currTrans.isChild())
				{
					currTrans.SetParent(nullptr);
				}
				else
				{
					Entity& entity = curr_scene.Get<Entity>(Index);
					auto it = std::find(layer.begin(), layer.end(), entity.EUID());

					layer.erase(it);
					layer.insert(layer.end(), entity.EUID());
				}

			}
			ImGui::EndDragDropTarget();

		}

		//Right click adding of entities in hierarchy window
		if (ImGui::BeginPopupContextWindow(0, true))
		{
			if (ImGui::MenuItem("Add Entity"))
			{
				Change newchange;
				newchange.entity = curr_scene.Add<Entity>();
				newchange.action = CREATING;
				EDITOR.History.AddEntityChange(newchange);
				SelectedEntityEvent selectedEvent{ newchange.entity };
				EVENTS.Publish(&selectedEvent);
			}

			Entity& ent = curr_scene.Get<Entity>(selectedEntity);
			auto& currEntity = curr_scene.Get<Transform>(selectedEntity);

			//add child entity to current selected entity
			if (ImGui::MenuItem("Add Child Entity")) {

				Change newchange;
				newchange.action = CREATING;

				Entity* Newentity = curr_scene.Add<Entity>();
				auto& newtransform = curr_scene.Get<Transform>(*Newentity);
				newtransform.SetParent(&currEntity);

				newchange.entity = Newentity;
				EDITOR.History.AddEntityChange(newchange);
				SelectedEntityEvent selectedEvent{ Newentity };
				EVENTS.Publish(&selectedEvent);

				newtransform.scale = Vector3(1.f, 1.f, 1.f);
				newtransform.translation = Vector3();
				newtransform.rotation = Vector3();
			}

			std::string name = "Delete Entity";
			if (selectedEntity != NON_VALID_ENTITY)
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					//Delete all children of selected entity as well
					//curr_scene.Destroy(ent);
					Change newchange;
					newchange.entity = &ent;
					EDITOR.History.AddEntityChange(newchange);
					SelectedEntityEvent selectedEvent{ 0 };
					EVENTS.Publish(&selectedEvent);
				}
			}
			else
			{
				ImGui::TextDisabled(name.c_str());
			}

			ImGui::Separator();
			if (ImGui::MenuItem("Add to Prefabs"))
			{
				SerializePrefab(ent, curr_scene);
			}

			ImGui::EndPopup();
		}

		ImGui::TreePop();
		
	}
	ImGui::EndChild();
	ImGui::End();
}

void EditorHierarchy::CallbackSelectedEntity(SelectedEntityEvent* pEvent)
{
	if (pEvent->pEntity)
		selectedEntity = pEvent->pEntity->EUID();
	else
		selectedEntity = NON_VALID_ENTITY;
}

void EditorHierarchy::Exit() {

}