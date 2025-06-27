static void handleClickInViewport(WindowData* winData){

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 cursorPosition = ImGui::GetMousePos();
    ImVec2 origin = ImGui::GetMainViewport()->Pos;
    
    if(
        ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().WantCaptureMouse && 
        cursorPosition.x >= viewport->Pos.x && cursorPosition.x <= viewport->Pos.x + viewport->Size.x &&
        cursorPosition.y >= viewport->Pos.y && cursorPosition.y <= viewport->Pos.y + viewport->Size.y
    ){
        if(winData->multiSelectionEnabled){
            std::cout << "MultiSelection is Enabled!" << std::endl;
            Vec2 relativePosition(cursorPosition.x - origin.x, cursorPosition.y - origin.y);
            Node* selected = s_Graph.getNodeAt(relativePosition, Config::selectionRadius, Config::nodeSize);

            if(selected) {
                auto it = std::find(s_selectedNodes.begin(), s_selectedNodes.end(), selected);
                if(it == s_selectedNodes.end()){
                    s_selectedNodes.push_back(selected);
                }else{
                    s_selectedNodes.erase(it);
                }
            }

            if(winData->keepMultiSelectionOpened){
                ImVec2 multiNodeEditorPosition = ImVec2(cursorPosition.x + 30, cursorPosition.y + 30);
                ImGui::SetNextWindowPos(multiNodeEditorPosition, ImGuiCond_Always);
                ImGui::OpenPopup("Node Editor");
                if(winData->editableNode) HelpMarker("<Multi Selection Mode is Enabled!>");
            }
        }
        
        else{
            s_targetEditable = nullptr;
            winData->editableNode = false;
            s_tempNode.position.x = cursorPosition.x - origin.x;
            s_tempNode.position.y = cursorPosition.y - origin.y;
            
            if(!s_targetEditable){
                
                s_targetEditable = s_Graph.getNodeAt(s_tempNode.position, Config::selectionRadius, Config::nodeSize);
                s_tempNode.setNodeType(NodeType::None);
            }
            if(s_targetEditable){ 
                winData->editableNode = true;
            }

            ImVec2 nodeEditorPosition = ImVec2(cursorPosition.x + 30, cursorPosition.y + 30);
            ImGui::SetNextWindowPos(nodeEditorPosition, ImGuiCond_Always);
            ImGui::OpenPopup("Node Editor");
            if(winData->editableNode) HelpMarker("<Drag to Edit Node Position>");
        }
    }
    if (ImGui::BeginPopup("Node Editor")) {
        if(winData->multiSelectionEnabled) { showMultiNodeEditor(winData); }
        else{
            winData->editableNode ? showNodeEditor(winData, s_targetEditable, true) 
        : showNodeEditor(winData, &s_tempNode, false);
        }
        ImGui::EndPopup();
    }

    //////////  Now a little bit of edge handling, finally..
    ////////// Edge Graphics Early Tests
    static bool dragging = false;
    static Vec2 dragStart;

    ImDrawList* drawList = ImGui::GetBackgroundDrawList(viewport);

    ImVec2 mousePos = ImGui::GetIO().MousePos;
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        if (!ImGui::GetIO().WantCaptureMouse) {
            dragging = true;
            dragStart = {cursorPosition.x - origin.x, cursorPosition.y - origin.y};
        }
    }

    if (dragging && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        Node* dragStartNode = s_Graph.getNodeAt(dragStart, Config::selectionRadius * 1.5f, Config::nodeSize);
        if(dragStartNode) std::cout << "Dragging from Node" << reinterpret_cast<uintptr_t>(dragStartNode) << std::endl;

        for (int i = 0; i < 4; ++i) {
            float thickness = 1.5f + i; 
            drawList->AddLine({dragStart.x + origin.x, dragStart.y + origin.y}, ImGui::GetIO().MousePos, ImGui::ColorConvertFloat4ToU32(Config::s_edgeColor), thickness);
        }
    }
    if (dragging && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
        dragging = false;
    }

    bool isPopupOpenNow = ImGui::IsPopupOpen("Node Editor");

    if(s_WasPopupOpen && !isPopupOpenNow){
        if(winData->editableNode){
            *s_targetEditable = s_copiedNode;
            winData->editableNode = false;
            s_targetEditable = nullptr;
            s_CopiedOnceFlag = false;
            s_selectedNodes.clear();
            s_winData.keepMultiSelectionOpened = false;
            s_winData.multiSelectionEnabled = false;
            s_tempPosition = {0, 0};
            s_tempPositionCaptured = false;
        }
    }

    s_WasPopupOpen = isPopupOpenNow;

}

