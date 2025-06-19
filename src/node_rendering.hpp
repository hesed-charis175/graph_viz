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
            
            s_targetEditable = s_Graph.getNodeAt(s_tempNode.position, Config::selectionRadius, Config::nodeSize);
            s_tempNode.setNodeType(NodeType::None);
        }
        if(s_targetEditable){ 
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

    bool isPopupOpenNow = ImGui::IsPopupOpen("Node Editor");

    if(s_WasPopupOpen && !isPopupOpenNow){
        if(winData->editableNode){
            *s_targetEditable = s_copiedNode;
            winData->editableNode = false;
            s_targetEditable = nullptr;
            s_CopiedOnceFlag = false;
        }
    }

    s_WasPopupOpen = isPopupOpenNow;

}

