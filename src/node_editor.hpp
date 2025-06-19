static void showNodeEditor(WindowData* winData, Node* editableNode, bool editable){
    if (editable && !s_CopiedOnceFlag) {
        s_copiedNode.position = editableNode->position;
        s_copiedNode.setNodeType(editableNode->getNodeType());
        if(s_Graph.end) s_localGrEnd = *s_Graph.end;
        if(s_Graph.start) s_localGrStart = *s_Graph.start;
        s_CopiedOnceFlag = true;
    }
    if(winData->editableNode && Config::allowNodeDragging){
        if(ImGui::IsMouseDragging(0, 0.1f)){
            editableNode->position.x += ImGui::GetIO().MouseDelta.x;
            editableNode->position.y += ImGui::GetIO().MouseDelta.y;
        }
    }

    ImGui::SeparatorText("Node Type");

    const char* nodeTypes[] = {"- Default", "Start", "Intermediate", "End"};
    int currentType = static_cast<int>(editableNode->getNodeType());

    ImGui::ListBox("Node Type", &currentType, nodeTypes, IM_ARRAYSIZE(nodeTypes), 4);
    ImGui::SameLine();
    
    HelpMarker(Strings::nodeHelpMarker);  

    int _x = editableNode->position.x, _y = editableNode->position.y;

    ImGui::SeparatorText("Node Position");
    ImGui::InputInt("X coordinate", &_x, 10);

    if (_x > Config::windowWidth - Config::nodeSize) _x = Config::windowWidth - Config::nodeSize;
    if (_x < 0) _x = 0;

    
    ImGui::InputInt("Y coordinate", &_y, 10);
    
    if (_y > Config::windowHeight - Config::nodeSize) _y = Config::windowHeight - Config::nodeSize;
    if (_y < 0) _y = 0;

    editableNode->position.x = static_cast<float>(_x);
    editableNode->position.y = static_cast<float>(_y);

    NodeType type;
    ImVec4 tmpNodeColor;
    switch(currentType){
        case 0:
            tmpNodeColor = Config::s_nodeColor;
            editableNode->setNodeType(NodeType::None);
            type = NodeType::None;
            break;
        case 1: 
            tmpNodeColor = Config::s_startColor;
            editableNode->setNodeType(NodeType::Start);
            type = NodeType::Start;
            break;
        case 2:
            tmpNodeColor = Config::s_intermediateColor;
            editableNode->setNodeType(NodeType::Intermediate);
            type = NodeType::Intermediate;
            break;
        case 3:
            tmpNodeColor = Config::s_endColor;
            editableNode->setNodeType(NodeType::End);
            type = NodeType::End;
            break;
 
    }  

    if (editableNode->position.x >= 0 && editableNode->position.y >= 0) {
        drawNode(*editableNode, tmpNodeColor);
        if(editable) drawNode(*editableNode, Config::s_selectionColor, true);
    }

    s_toLog += "[DEBUG/UI] Adding node from editableNode " + std::to_string(reinterpret_cast<uintptr_t>(editableNode)) + 
            "[id=" + std::to_string(editableNode->node_id) + "] type: " + std::to_string(static_cast<int>(type)) + " | " + editableNode->debugNodeTypeString + "\n";


    // Segmentation fault (core dumped) comes from here. I should probably find a way to handle this... What causes it.
    if(ImGui::Button("Cancel")){
        if(editable){
            s_targetEditable = nullptr;
            // editableNode->print();
            *editableNode = s_copiedNode;
            winData->editableNode = false;
            s_CopiedOnceFlag = false;
            editableNode = nullptr;
        }
        ImGui::CloseCurrentPopup();
        if(!editable) *editableNode = Node();
    }
    ImGui::SameLine();
    if(!editable){
        if(ImGui::Button("Add Node")){
            if (!s_Graph.isNodePtrValid(editableNode) && editable) {
        s_toLog += "[DEBUG] Editable pointer" + std::to_string(reinterpret_cast<uintptr_t>(editableNode)) + " is NOT valid in s_Graph.nodes! Possible stale pointer!\n";
    } else {
        s_toLog += "[DEBUG] Editable pointer" + std::to_string(reinterpret_cast<uintptr_t>(editableNode)) + " is valid in s_Graph.nodes! (Not yet in s_Graph)\n";
    }
            s_Graph.debugPrintState();        
            editableNode->setNodeType(NodeType::None);
            s_Graph.addNode(*editableNode);
            s_Graph.debugPrintState();
            s_Graph.setNodeType(&s_Graph.nodes.back(), type);
            s_Graph.enforceNodeTypeConsistency();
            
            winData->editableNode = false;
            ImGui::CloseCurrentPopup();
            editableNode = nullptr;
        }
    }else{
        if(ImGui::Button("Edit")){
if (!s_Graph.isNodePtrValid(editableNode) && editable) {
        s_toLog += "[DEBUG] Editable pointer" + std::to_string(reinterpret_cast<uintptr_t>(editableNode)) + " is NOT valid in s_Graph.nodes! Possible stale pointer!\n";
    } else {
        s_toLog += "[DEBUG] Editable pointer" + std::to_string(reinterpret_cast<uintptr_t>(editableNode)) + " is valid in s_Graph.nodes! (Not yet in s_Graph)\n";
    }
            s_Graph.debugPrintState();         
            s_Graph.setNodeType(editableNode, type);
            s_targetEditable = nullptr;
            ImGui::CloseCurrentPopup();
            winData->editableNode = false;
            s_CopiedOnceFlag = false;
            editableNode = nullptr;

        }
    }

}
