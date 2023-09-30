#include "Precompiled.h"
#include "EditorHeaders.h"
#include "Editor.h"
#include "Scene/Scene.h"
#include "Core/EventsManager.h"
#include "Scene/SceneManager.h"

void EditorHierarchy::Init() 
{
    //no selected entity at start
    selectedEntity = NON_VALID_ENTITY;
    EVENTS.Subscribe(this,&EditorHierarchy::CallbackSelectedEntity);
}

void EditorHierarchy::DisplayEntity(Engine::UUID euid)
{

	// ImGuiTreeNodeFlags_SpanAvailWidth

	ImGuiTreeNodeFlags NodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
		ImGuiTreeNodeFlags_OpenOnDoubleClick |
		ImGuiTreeNodeFlags_DefaultOpen;

	if (euid == selectedEntity)
	{
		NodeFlags |= ImGuiTreeNodeFlags_Selected;
	}

	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();



	Scene::Layer& layer = curr_scene.layer;
	Transform& currEntity = curr_scene.Get<Transform>(euid);

	if (currEntity.isLeaf())
	{
		NodeFlags |= ImGuiTreeNodeFlags_Bullet;
	}

	//Invisible button for drag drop reordering
	ImGui::InvisibleButton("##", ImVec2(ImGui::GetWindowContentRegionWidth(), 2.5f));

	//Drag drop reordering implementation
	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity"))
		{
			Engine::UUID childId = *static_cast<Engine::UUID*>(payload->Data);


			if (childId != euid)
			{
				Entity& currEntity = curr_scene.Get<Entity>(childId);
				Entity& targetEntity = curr_scene.Get<Entity>(euid);

				Transform& currTransform = curr_scene.Get<Transform>(currEntity);
				Transform& targetTransform = curr_scene.Get<Transform>(targetEntity);

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
							arr.insert(it, currEntity.EUID());
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
							parent.insert(it,currTransform.EUID());
						}
					}

				}
				//if target entity is a base node (no parent)
				else
				{
					//if current entity has a parent, delink it
					if (currTransform.isChild())
					{
						currTransform.SetParent(nullptr);
					}
					//delete instance of entity in container
					auto prev_it = std::find(layer.begin(), layer.end(), currEntity.EUID());
					layer.erase(prev_it);

					//reorder (reinsert) the current entity into new layer position
					auto it = std::find(layer.begin(), layer.end(), targetEntity.EUID());
					layer.insert(it, currEntity.EUID());
				}

			}

		}
		ImGui::EndDragDropTarget();
	}

	auto EntityName = curr_scene.Get<Tag>(euid).name.c_str();
	bool open = ImGui::TreeNodeEx(EntityName, NodeFlags);

	//select entity from hierarchy
	if (ImGui::IsItemClicked())
	{
        SelectedEntityEvent selectedEvent{&curr_scene.Get<Entity>(euid)};
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

			Transform& currEntity = curr_scene.Get<Transform>(childId);
			Transform& targetEntity = curr_scene.Get<Transform>(euid);

			if (currEntity.isLeaf())
			{
				if (childId != euid)
				{
					currEntity.SetParent(&targetEntity);
				}
			}
			else
			{
				if (!currEntity.isEntityChild(targetEntity))
				{
					currEntity.SetParent(&targetEntity);
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
	Scene& curr_scene = SceneManager::Instance().GetCurrentScene();

	bool sceneopen = ImGui::TreeNodeEx(curr_scene.sceneName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

	Scene::Layer& layer = curr_scene.layer;
	if (sceneopen)
	{
		for (Engine::UUID euid : layer)
		{
			if (!curr_scene.Get<Transform>(euid).isChild())
			{
				//Recursive function to display entities in a hierarchy tree
				DisplayEntity(euid);
			}
		}

		ImGui::InvisibleButton("##", ImGui::GetContentRegionAvail());

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
                SelectedEntityEvent selectedEvent{ curr_scene.Add<Entity>() };
                EVENTS.Publish(&selectedEvent);
			}

			std::string name = "Delete Entity";
			if (selectedEntity != NON_VALID_ENTITY)
			{
				if (ImGui::MenuItem(name.c_str()))
				{
					Entity& ent = curr_scene.Get<Entity>(selectedEntity);
					//Delete all children of selected entity as well
					auto& currEntity = curr_scene.Get<Transform>(selectedEntity);
					for (auto child : currEntity.child)
					{
						curr_scene.Destroy(child);
					}
					curr_scene.Destroy(ent);
					SelectedEntityEvent selectedEvent{0};
					EVENTS.Publish(&selectedEvent);
				}
			}
			else
			{
				ImGui::TextDisabled(name.c_str());
			}

			ImGui::EndPopup();
		}

		ImGui::TreePop();
	}
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