static void updateNodes(WindowData* winData){
    for(auto node : s_Graph.nodes){
        ImVec4 tmpNodeColor;
        switch(node.getNodeType()){
            case NodeType::Start: 
                tmpNodeColor = Config::s_startColor;
                break;
            case NodeType::Intermediate:
                tmpNodeColor = Config::s_intermediateColor;
                break;
            case NodeType::End:
                tmpNodeColor = Config::s_endColor;
                break;
            default:
                tmpNodeColor = Config::s_nodeColor;
                break;
        }
        if (node.position.x >= 0 && node.position.y >= 0) {
            drawNode(node, tmpNodeColor);
        }
    }
}

void drawNode(Node node, ImVec4& color, bool outline = false){
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImVec2 origin = ImGui::GetMainViewport()->Pos;
    ImVec2 pos = ImVec2(
        node.position.x + origin.x, 
        node.position.y + origin.y

    );
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

    if(outline){
        drawList->AddCircle(pos, Config::nodeSize + Config::edgeThickness, colorU32);
    }else{
        ImVec2 textOffset = {15.0f, 0.0f};
        ImVec2 textPosition = {pos.x + textOffset.x, pos.y + textOffset.y};

    if(Config::displayPosition){
            ImGui::GetBackgroundDrawList()->AddText(textPosition, IM_COL32(255, 255, 255, 255),
            ("(" + std::to_string((int)node.position.x) + ", " + std::to_string((int)node.position.y) + ")").c_str());
    }

        for(int i = 0; i < Config::glowIterations; i++){
            float glowRadius = Config::nodeSize + i * Config::glowFactor;
            float alpha = (Config::glowFactor - i * 0.1f) * 0.2f * Config::glowFactor;
            ImVec4 glowColor = ImVec4(color.x, color.y, color.z, alpha);
            ImU32 glowU32 = ImGui::ColorConvertFloat4ToU32(glowColor);
            drawList->AddCircleFilled(pos, glowRadius, glowU32);
        }

    drawList->AddCircleFilled(pos, Config::nodeSize, colorU32);
    }
}

static void handleClickInViewport(WindowData* winData){

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 cursorPosition = ImGui::GetMousePos();
    ImVec2 origin = ImGui::GetMainViewport()->Pos;
    
    if(
        ImGui::IsMouseClicked(0) && !ImGui::GetIO().WantCaptureMouse && 
        cursorPosition.x >= viewport->Pos.x && cursorPosition.x <= viewport->Pos.x + viewport->Size.x &&
        cursorPosition.y >= viewport->Pos.y && cursorPosition.y <= viewport->Pos.y + viewport->Size.y
    ){
        s_targetEditable = nullptr;
        winData->editableNode = false;
        s_tempNode.position.x = cursorPosition.x - origin.x;
        s_tempNode.position.y = cursorPosition.y - origin.y;
        
        if(!s_targetEditable){
            
            s_targetEditable = s_Graph.getNodeAt(s_tempNode.position);
            s_tempNode.setNodeType(NodeType::None);
        }
        if(s_targetEditable){ 
            // std::cout << "[DEBUG] Found Node : ";
            // s_targetEditable->print();
            winData->editableNode = true;
            s_selectedNodes.clear();
            s_selectedNodes.push_back(s_targetEditable);
        }

        ImVec2 nodeEditorPosition = ImVec2(cursorPosition.x + 30, cursorPosition.y + 30);
        ImGui::SetNextWindowPos(nodeEditorPosition, ImGuiCond_Always);
        ImGui::OpenPopup("Node Editor");
        if(winData->editableNode) HelpMarker("<Drag to Edit Node Position>");
    }
    if (ImGui::BeginPopup("Node Editor")) {
        winData->editableNode ? showNodeEditor(winData, s_targetEditable, true) 
        : showNodeEditor(winData, &s_tempNode, false);
        ImGui::EndPopup();
    }

}
