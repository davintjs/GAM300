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

void EditorBehaviourTreeEditor::Init() {
    m_Context = ed::CreateEditor();
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::ControlFlow, "ControlNode1.bt", ImVec2(10, 10)));
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::Decorator, "DecoratorNode1.bt", ImVec2(210, 60)));
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::Leaf, "BehaviorTree1.bt", ImVec2(100, 150)));
    m_Nodes.push_back(CreateNode(uniqueId, NodeType::Leaf, "BehaviorTree2.bt", ImVec2(300, 150)));
}

void EditorBehaviourTreeEditor::Update(float dt) {
    UNREFERENCED_PARAMETER(dt);
    ed::SetCurrentEditor(m_Context);
    ed::Begin("Behaviour Tree Editor");

    // Draw nodes
    for (auto& node : m_Nodes) {
        DrawNode(node);
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

    if (m_FirstFrame)
        ed::NavigateToContent(0.0f);

    ed::SetCurrentEditor(nullptr);
    m_FirstFrame = false;
}

void EditorBehaviourTreeEditor::Exit() {
    ed::DestroyEditor(m_Context);
}

EditorBehaviourTreeEditor::Node EditorBehaviourTreeEditor::CreateNode(int& id, NodeType type, const std::string& filename, const ImVec2& position) {
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
    ed::BeginNode(node.id);

    ImGui::Columns(1, "header", false);
    ImGui::NextColumn();
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

    ImGui::Dummy(ImVec2(20, 0));    
    ed::BeginPin(node.outputPinId, ed::PinKind::Output);
    ImGui::Text("Output ->");
    ed::EndPin();


    // Add drawing logic for pins, etc.
    ed::EndNode();
}