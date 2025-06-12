static void ShowMenuFile()
{
    if (ImGui::MenuItem("(File Management)", NULL, false, false)) {}
    if (ImGui::MenuItem("New")) {}
    if (ImGui::MenuItem("Open", "Ctrl+O")) {}
    if (ImGui::BeginMenu("Open Recent"))
    {
        
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // TODO: Add stack structure to handle recent files based on json saved node configurations, parameters, edges... etc
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        ImGui::EndMenu();
    }
    if (ImGui::MenuItem("Save", "Ctrl+S")) {}
    if (ImGui::MenuItem("Save As..")) {}

    ImGui::Separator();
    if (ImGui::MenuItem("Quit", "Alt+F4")) {}
}

static void showAppMainMenuBar(WindowData* winData)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ShowMenuFile();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {} 
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

}

static void showWindowMenuBar(WindowData* winData)
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("Tools"))
        {
            if ( ImGui::MenuItem("Metrics/Debugger", NULL, &winData->showMetrics, true)) {}
            if ( ImGui::MenuItem("Log Window")) {}
            if (ImGui::MenuItem("About"))
            {
                winData->showAbout = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void showStyleEditor(WindowData* winData){
    if(!ImGui::Begin("Appearance Editor", &winData->showStyleEditor)){
        ImGui::End();
        return;
    }

    ImGui::Text("Edit appearance");

    if(ImGui::CollapsingHeader("Viewport")){
        ImGui::ColorEdit4("Background Color", (float*)&Config::s_viewportColor);
        ImGui::SameLine();
        HelpMarker(
            Default_Strings::colorHelpMarker
        );
        ImGui::Spacing();

        ImGui::SliderFloat("Zoom Factor", &Config::zoomFactor, Config::zoomMin, Config::zoomMax);

    }

    if(ImGui::CollapsingHeader("Graph")){

        static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;

        ImGui::SeparatorText("Nodes");


        if(ImGui::TreeNodeEx("Node Settings", baseFlags)){

            if(ImGui::TreeNodeEx("General", baseFlags)){
                
                ImGui::SliderInt("Node Size", &Config::nodeSize, Config::minNodeSize, Config::maxNodeSize);

                ImGui::SliderFloat("Glow Factor", &Config::glowFactor, Config::minGlowFactor, Config::maxGlowFactor);

                ImGui::SliderInt("Glow Iteration", &Config::glowIterations, 0, 20);
            
                ImGui::Checkbox("Allow Node Dragging", &Config::allowNodeDragging);

                ImGui::Checkbox("Display Node Coordinates", &Config::displayPosition);

                ImGui::TreePop();

            }
            if(ImGui::TreeNodeEx("Colors", baseFlags)){
                ImGui::ColorEdit4("Default Color", (float*)&Config::s_nodeColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();
        
        
                ImGui::ColorEdit4("Start Color", (float*)&Config::s_startColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();
        
                ImGui::ColorEdit4("Intermediate Color", (float*)&Config::s_intermediateColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();
        
                ImGui::ColorEdit4("Goal Color", (float*)&Config::s_endColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::TreePop();

            }
        
            ImGui::TreePop();
        }

        ImGui::SeparatorText("Edges");

        if(ImGui::TreeNodeEx("Edge Settings", baseFlags)){
            if(ImGui::TreeNodeEx("General", baseFlags)){
                ImGui::SliderInt("Edge Thickness", &Config::edgeThickness, 1.0f, 5.0f);
                ImGui::TreePop();


            }

            if(ImGui::TreeNodeEx("Colors", baseFlags)){
                ImGui::ColorEdit4("Edge Color", (float*)&Config::s_edgeColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::ColorEdit4("Path 1 Color", (float*)&Config::s_pathColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::ColorEdit4("Path 2 Color", (float*)&Config::s_auxPathColor);
                ImGui::SameLine();
                HelpMarker(
                    Default_Strings::colorHelpMarker
                );
                ImGui::Spacing();
                ImGui::Spacing();

                ImGui::TreePop();

            }

            ImGui::TreePop();

        }
    }

    ImGui::End();
}

static void showNodeEditor(WindowData* winData, Node* editableNode, bool editable){
    if (editable && !s_CopiedOnceFlag) {
        s_copiedNode.position = editableNode->position;
        s_copiedNode.setNodeType(editableNode->getNodeType());
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
    std::cout << currentType << std::endl;

    ImGui::ListBox("Node Type", &currentType, nodeTypes, IM_ARRAYSIZE(nodeTypes), 4);
    ImGui::SameLine();
    
    HelpMarker(Default_Strings::nodeHelpMarker);  

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

    // s_copiedNode.print();

    ImVec4 tmpNodeColor;
    switch(currentType){
        case 0:
            tmpNodeColor = Config::s_nodeColor;
            editableNode->setNodeType(NodeType::None);
            break;
        case 1: 
            tmpNodeColor = Config::s_startColor;
            editableNode->setNodeType(NodeType::Start);
            break;
        case 2:
            tmpNodeColor = Config::s_intermediateColor;
            editableNode->setNodeType(NodeType::Intermediate);
            break;
        case 3:
            tmpNodeColor = Config::s_endColor;
            editableNode->setNodeType(NodeType::End);
            break;
 
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: Make the glowing effect modular and dependent on the config file static variables and adding some kind of control for them in the style editor ///
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    if (editableNode->position.x >= 0 && editableNode->position.y >= 0) {
        drawNode(*editableNode, tmpNodeColor);
        if(editable) {
            drawNode(*editableNode, Config::s_edgeColor, true);
        }
    }

    if(ImGui::Button("Cancel")){
        if(editable){
            s_targetEditable = nullptr;
            // editableNode->print();
            *editableNode = s_copiedNode;
            winData->editableNode = false;
            s_CopiedOnceFlag = false;
        }
        ImGui::CloseCurrentPopup();
        if(!editable) *editableNode = Node();
    }
    ImGui::SameLine();
    if(!editable){
        if(ImGui::Button("Add Node")){
            s_Graph.addNode(*editableNode);
            editableNode->~Node();
            winData->editableNode = false;
            ImGui::CloseCurrentPopup();
        }
    }else{
        if(ImGui::Button("Edit")){
            s_targetEditable = nullptr;
            ImGui::CloseCurrentPopup();
            winData->editableNode = false;
            s_CopiedOnceFlag = false;
        }
    }

}
static void showAppSimpleOverlay(WindowData* winData) {
    static int location = 0;

    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoDecoration | 
    ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | 
    ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

    const float PAD = 10.0f;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 work_pos = viewport->WorkPos;
    ImVec2 work_size = viewport->WorkSize;
    ImVec2 window_pos, window_pos_pivot;
    window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
    window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
    window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
    window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
    ImGui::SetNextWindowViewport(viewport->ID);
    winFlags |= ImGuiWindowFlags_NoMove;

    ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("IO overlay", &winData->showAppSimpleOverlay, winFlags))
    {
        ImGui::Text("IO overlay");
        ImGui::Separator();
        if (ImGui::IsMousePosValid()) ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        else ImGui::Text("Mouse Position: <invalid>");
            

        updateKeyBuffer();

        for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_COUNT; key++) {
            if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {
                s_KeyBuffer.push_back({static_cast<ImGuiKey>(key), ImGui::GetKeyName(static_cast<ImGuiKey>(key)), std::chrono::steady_clock::now()});
                break;
            }
        }

        for(const auto& entry : s_KeyBuffer){
            ImGui::Separator();
            ImGui::Text("Key Pressed : <%s>", entry.name.c_str());
        }

        if(winData->editableNode){
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("<Edit Mode>");
        }
    }
    ImGui::End();

}

void showAbout(WindowData* winData){
    if(!ImGui::Begin("About Me", &winData->showAbout)){
        ImGui::End();
        return;
    }

    ImGui::Text("Graph Visualizer is a real-time visualization tool built in C++ with ImGui,"
        "\ndesigned to help users interactively explore graph structures, modify parameters,"
        "\nand test pathfinding algorithms. It provides a map-based interface where users can "
        "\nadjust the nature of node connectivity, set start and end points, and analyze algorithm performance.");

    

    static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;
    
    if(ImGui::TreeNodeEx("Features", baseFlags)){
        if(ImGui::TreeNodeEx("Map-based Graph Representation", baseFlags)){

            ImGui::BulletText("city-like graph structures");
            ImGui::BulletText("Configurable node connectivity settings");
            ImGui::BulletText("Click to set start, end, or intermediate nodes dynamically");
            ImGui::BulletText("If a new end point is assigned, the previous end becomes an intermediate node");

            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Customizable Appearance", baseFlags)){

            ImGui::BulletText("Adjust colors for nodes, edges, solution paths and backgrounds");
            ImGui::BulletText("Highlight keypoints such as start, intermediate and end points");

            ImGui::TreePop();
        }

        if(ImGui::TreeNodeEx("Algorithm Testing and Logs", baseFlags)){

            ImGui::BulletText("Supports BFS, DFS, A*, RRT, UCS, Greedy (eulerian heuristics) algorithms");
            ImGui::BulletText("Execution time tracking and graphs to analyse algorithm efficiency");
            ImGui::TreePop();
        }
        ImGui::TreePop();
    }
    ImGui::Spacing();
    ImGui::Text("Author: Bayiha Hesed Charis.");

    ImGui::End();
    
}

static WindowData s_winData;

void showConfigPanelFull(bool* p_open)

{

    if(s_winData.showMainMenuBar) showAppMainMenuBar(&s_winData);
    if(s_winData.showStyleEditor) showStyleEditor(&s_winData);
    if(s_winData.showAppSimpleOverlay) showAppSimpleOverlay(&s_winData);
    if(s_winData.showAbout) showAbout(&s_winData);
    if (s_winData.evaluateClick) handleClickInViewport(&s_winData);
    if(s_winData.updateNodes) updateNodes(&s_winData);


    const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x + 20, mainViewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    if(!ImGui::Begin("Configuration Panel", NULL, ImGuiWindowFlags_MenuBar)){
        ImGui::End();
        return;
    }

    const float labelWidthBase = ImGui::GetFontSize() * 12;
    const float labelWidthMax = ImGui::GetContentRegionAvail().x * 0.40f;
    const float labelWidth = IM_MIN(labelWidthBase, labelWidthMax);
    ImGui::PushItemWidth(-labelWidth);

    showWindowMenuBar(&s_winData);

    ImGui::Text("Graoh Viewer v(%s)", Config::version);
    ImGui::Spacing();

    if(ImGui::Button("Appearance Editor")){
        s_winData.showStyleEditor = true;
    }
    ImGui::Spacing();
    ImGui::Spacing();


    ImGui::End();
}


