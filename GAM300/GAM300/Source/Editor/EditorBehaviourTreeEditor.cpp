/*!***************************************************************************************
\file			EditorBehaviourTreeEditor.cpp
\project
\author         Joseph Ho

\par			Course: GAM300
\date           15/01/2024

\brief
    This file contains the definitions of the functions used for the Editor debugger window.

All content © 2023 DigiPen Institute of Technology Singapore. All rights reserved.
******************************************************************************************/

#include "Precompiled.h"
#include "EditorHeaders.h"
#include "Core/EventsManager.h"

namespace ed = ax::NodeEditor;

ed::NodeId selectedNode = 0;

using BT = EditorBehaviourTreeEditor;

void ImGuiEx_BeginColumn() {
    ImGui::BeginGroup();
}

void ImGuiEx_NextColumn() {
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
}

void ImGuiEx_EndColumn() {
    ImGui::EndGroup();
}

void AddSpacing(float width) {
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(width, 0));
    ImGui::SameLine();
}

void BT::DrawNodeHierarchyWindow() {
    ImGui::Begin("Node Hierarchy");
    ImGui::BeginChild("ScrollingRegion", ImVec2(0, -20.f), false);

    bool open = ImGui::TreeNodeEx("Behaviours", ImGuiTreeNodeFlags_DefaultOpen);

    if (open) {
        for (auto& node : m_Nodes) {
            DrawNodeHierarchy(node, 0);
        }
        //Right click adding of entities in hierarchy window
        if (ImGui::BeginPopupContextWindow(0, true))
        {
            if (ImGui::MenuItem("Add Behaviour"))
            {

            }
            ImGui::EndPopup();
        }
    }

    ImGui::TreePop();
    ImGui::EndChild();
    ImGui::End();

}



void BT::DrawNodeHierarchy(Node& currentNode, int depth) {
    uintptr_t nodeInt = reinterpret_cast<uintptr_t>(currentNode.id.AsPointer());
    ImGui::PushID(nodeInt);
    ImGui::Indent(depth * 20.0f);
    // Colored rectangle to indicate node type
    ImGui::ColorButton("##NodeType", ImVec4(ImColor(currentNode.color)), ImGuiColorEditFlags_NoTooltip);
    ImGui::SameLine();
    ImGui::Selectable(currentNode.filename.c_str());
    if (ImGui::BeginDragDropSource()) {
        ImGui::SetDragDropPayload("Node", &currentNode, sizeof(BT::Node));
        ImGui::Text(currentNode.filename.c_str());
        ImGui::EndDragDropSource();
    }
    ImGui::Unindent(depth * 20.0f);
    ImGui::PopID();

}


void EditorBehaviourTreeEditor::Init() {
    m_Context = ed::CreateEditor();
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::ControlFlow, "ControlNode1.bt", ImVec2(10, 10)));
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::Decorator, "DecoratorNode1.bt", ImVec2(210, 60)));
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::Leaf, "BehaviorTree1.bt", ImVec2(100, 150)));
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::Leaf, "BehaviorTree2.bt", ImVec2(300, 150)));
}

void EditorBehaviourTreeEditor::Update(float dt) {
    UNREFERENCED_PARAMETER(dt);
  
    if (editorOpen) {
        // Set the current editor context
        ed::SetCurrentEditor(m_Context);

        if (ImGui::Begin("Behaviour Tree Editor", &editorOpen, ImGuiWindowFlags_MenuBar)) {

            if (ImGui::BeginMenuBar())
            {
                //File Menu functionality
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("New")) {

                    }

                    // Opening of files using file dialogs
                    if (ImGui::MenuItem("Open")) {

                    }

                    // Saving of scene files only if an active scene is open, using file dialogs
                    if (ImGui::MenuItem("Save")) {

                    }

                    // Save as functionality using file dialogs
                    if (ImGui::MenuItem("Save As.."))
                    {
                        std::string filepath = FileDialogs::SaveFile("Json (*.json)\0*.json\0");
                        //Save File
                    }

                    if (ImGui::MenuItem("Close"))
                    {
                        editorOpen = false;
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            // Hierarchy Tree Window on the Left
            ImGui::BeginChild("HierarchyTree", ImVec2(250, 600), true, ImGuiWindowFlags_HorizontalScrollbar);
            ImGui::Text("Node Hierarchy");
            for (auto& node : m_Nodes) {
                DrawNodeHierarchy(node, 0);
            }

            // Right-click adding of entities in hierarchy window
            if (ImGui::BeginPopupContextWindow(0, true)) {
                if (ImGui::MenuItem("Add Behaviour")) {
                    // Handle adding behavior
                }
                ImGui::EndPopup();
            }


            ImGui::EndChild(); // End Hierarchy Tree Window

            ImGui::SameLine();

            // Main Editor Window on the Right
            ImGui::BeginChild("MainEditor", ImVec2(800, 600), false, ImGuiWindowFlags_HorizontalScrollbar);
            ed::Begin("Behaviour Tree Editor");

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Node"))
                {
                    Node* node = static_cast<Node*>(payload->Data);
                   
                    m_Nodes.push_back(CreateNode(uniqueId, node->type, node->filename, ed::ScreenToCanvas(ImGui::GetCursorPos())));
                }
                ImGui::EndDragDropTarget();
            }

            // Draw nodes
            for (auto& node : m_Nodes) {
                DrawNode(node);

                if (ImGui::IsItemClicked()) {
                    ed::DeselectNode(selectedNode);
                    selectedNode = node.id;
                    ed::SelectNode(selectedNode, true);
                }
            }

            // Draw links
            for (auto& linkInfo : m_Links) {
                ed::Link(linkInfo.Id, linkInfo.InputId, linkInfo.OutputId);
            }

            // Handle interactions (creation, deletion)
            if (ed::BeginCreate()) {
                ed::PinId inputPinId, outputPinId;
                if (ed::QueryNewLink(&inputPinId, &outputPinId)) {
                    if (inputPinId && outputPinId) {
                        if (ed::AcceptNewItem()) {
                            m_Links.push_back({ ed::LinkId(m_NextLinkId++), inputPinId, outputPinId });
                            ed::Link(m_Links.back().Id, m_Links.back().InputId, m_Links.back().OutputId);
                        }
                    }
                }
            }
            ed::EndCreate();

            if (ed::BeginDelete()) {

                ed::NodeId deletedNodeId;

                while (ed::QueryDeletedNode(&deletedNodeId)) {
                    // Remove the node from m_Nodes
                    auto nodeIt = std::remove_if(m_Nodes.begin(), m_Nodes.end(),
                        [deletedNodeId](const Node& node) { return node.id == deletedNodeId; });

                    if (nodeIt != m_Nodes.end()) {
                        m_Nodes.erase(nodeIt, m_Nodes.end());
                    }
                }

                ed::LinkId deletedLinkId;
                while (ed::QueryDeletedLink(&deletedLinkId)) {
                    if (ed::AcceptDeletedItem()) {
                        m_Links.erase(std::remove_if(m_Links.begin(), m_Links.end(), [deletedLinkId](LinkInfo& link) {
                            return link.Id == deletedLinkId;
                            }), m_Links.end());
                    }
                }
            }
            ed::EndDelete();

            ed::End();
            ImGui::EndChild(); // End Main Editor Window

            if (m_FirstFrame)
                ed::NavigateToContent(0.0f);

            // Reset the current editor context to nullptr after finishing interactions
            ed::SetCurrentEditor(nullptr);

            m_FirstFrame = false;
        }

        ImGui::End();
    }

    // Ensure that the editor context is properly destroyed
    if (!editorOpen) {
        ed::DestroyEditor(m_Context);
        m_Context = nullptr;
    }

}

void EditorBehaviourTreeEditor::Exit() {
    ed::DestroyEditor(m_Context);
}

BT::Node EditorBehaviourTreeEditor::CreateNode(int& id, NodeType type, const std::string& filename, const ImVec2& position) {
    switch (type) {
    case NodeType::ControlFlow:
        return ControlFlowNode(id, filename, position);
    case NodeType::Decorator:
        return DecoratorNode(id, filename, position);
    case NodeType::Leaf:
        return LeafNode(id, filename, position);
    }
}

void EditorBehaviourTreeEditor::DrawNode(Node& node) {
    //ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(node.color));
    ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(node.color));
    //ed::PushStyleColor(ed::StyleColor_PinRect, ImColor(60, 180, 255, 150));

    ed::BeginNode(node.id);

    ImGui::Text(node.filename.c_str());

    switch (node.type) {
    case NodeType::ControlFlow:
        ImGui::Text("Control Flow Node");
        break;
    case NodeType::Decorator:
        ImGui::Text("Decorator Node");
        break;
    case NodeType::Leaf: {
        ImGui::Text("Leaf Node");
        break;
    }
    }

    ed::BeginPin(node.inputPinId, ed::PinKind::Input);
    ImGui::Text("<- Input");
    ed::EndPin();

    AddSpacing(5.f);

    ed::BeginPin(node.outputPinId, ed::PinKind::Output);
    ImGui::Text("Output ->");
    ed::EndPin();

    // Add drawing logic for pins, etc.
    ed::EndNode();

    // Pop the style colors
    ed::PopStyleColor();
}
