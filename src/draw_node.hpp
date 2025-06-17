static void drawNode(Node node, ImVec4& color, bool outline = false){
    ImDrawList* drawList = ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());

    if(!drawList){
        std::cout << "[DEBUG]   DrawList is null!" << std::endl;
        return;
    }
    
    ImVec2 origin = ImGui::GetMainViewport()->Pos;
    ImVec2 pos = ImVec2(
        node.position.x + origin.x, 
        node.position.y + origin.y
        
    );
    ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);
    

    if(outline){
        drawList->AddCircle(pos, Config::nodeSize + Config::edgeThickness, colorU32, 0, 2.0f);
    }else{
        ImVec2 textOffset = {Config::s_textOffsetX, Config::s_textOffsetY};
        ImVec2 textPosition = {pos.x + textOffset.x, pos.y + textOffset.y};
        
        if(Config::displayPosition){
            ImGui::GetBackgroundDrawList(ImGui::GetMainViewport())->AddText(textPosition, IM_COL32(255, 255, 255, 255),
            ("(" + std::to_string((int)node.position.x) + ", " + std::to_string((int)node.position.y) + ")").c_str());
        }
        
        drawList->AddCircle(pos, Config::nodeSize + Config::edgeThickness, ImGui::ColorConvertFloat4ToU32(Config::s_edgeColor));
        
        for(int i = 0; i < Config::glowNodeIterations; i++){
            float glowRadius = Config::nodeSize + i * Config::glowNodeFactor;
            float alpha = (Config::glowNodeFactor - i * 0.1f) * 0.2f * Config::glowNodeFactor;
            ImVec4 glowColor = ImVec4(color.x, color.y, color.z, alpha);
            ImU32 glowU32 = ImGui::ColorConvertFloat4ToU32(glowColor);
            drawList->AddCircleFilled(pos, glowRadius, glowU32);
        }
        
        drawList->AddCircleFilled(pos, Config::nodeSize, colorU32);
        // std::cout << "Drawing now" << std::endl;
    }
}

static void updateNodes(WindowData* winData){
    for(auto node : s_Graph.nodes){
        ImVec4 tmpNodeColor;
        std::cout << "[DEBUG]" << node.debugNodeTypeString << std::endl;
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
            case NodeType::None:
                tmpNodeColor = Config::s_nodeColor;
                break;
        }
        if (node.position.x >= 0 && node.position.y >= 0) {
            drawNode(node, tmpNodeColor);
        }
    }

    // if(s_Graph.start) s_Graph.start->print();
}

