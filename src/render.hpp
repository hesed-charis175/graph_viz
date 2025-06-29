#include "io_overlay.hpp"
#include "menu_bar.hpp"
#include "style_editor.hpp"
#include "show_about.hpp"
#include "draw_node.hpp"
#include "node_editor.hpp"
#include "node_rendering.hpp"
#include "viewport.hpp"

static void showWindowMenuBar(WindowData* winData)
{
    if(ImGui::BeginMenuBar())
    {
        if(ImGui::BeginMenu("Tools"))
        {
            if ( ImGui::MenuItem("Metrics/Debugger", NULL, &winData->showMetrics, true)) {}
            if ( ImGui::MenuItem("Graph Log Window", NULL, &winData->showDebugLogs, true)) {}
            if ( ImGui::MenuItem("Node Debug List", NULL, &winData->showNodeDebugList, true)) {}
            if (ImGui::MenuItem("About"))
            {
                winData->showAbout = true;
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}

void showPanel(WindowData* winData){
    const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(mainViewport->WorkPos.x + 20, mainViewport->WorkPos.y + 20), ImGuiCond_Once);
    ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

    if(!ImGui::Begin("Panel", NULL, ImGuiWindowFlags_MenuBar)){
        ImGui::End();
        return;
    }

    const float labelWidthBase = ImGui::GetFontSize() * 12;
    const float labelWidthMax = ImGui::GetContentRegionAvail().x * 0.40f;
    const float labelWidth = IM_MIN(labelWidthBase, labelWidthMax);
    ImGui::PushItemWidth(-labelWidth);

    showWindowMenuBar(&s_winData);

    ImGui::Text("Graph Viewer v(%s)", Strings::version);
    ImGui::Spacing();

    if(ImGui::BeginTabBar("Panel", ImGuiTabBarFlags_None)){
        if (ImGui::BeginTabItem("Style")){
            ImGui::Text("This is the Style Editor Tab. It controls the appearance.\n");
            ImGui::Spacing();
            ImGui::Spacing();
            showStyleEditor(winData);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Map"))
            {
                ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
                ImGui::EndTabItem();
            }

        ImGui::EndTabBar();
    }

    ImGui::Spacing();
    ImGui::Spacing();


    ImGui::End();
}



void graphVisualizerRender(bool* p_open){
    Config::windowHeight = ImGui::GetMainViewport()->Size.y;
    Config::windowWidth = ImGui::GetMainViewport()->Size.x;


    if(Config::showGrids) {showGrid(&s_winData); }
    if(s_winData.showMainMenuBar) showAppMainMenuBar(&s_winData);
    if(s_winData.showStyleEditor) showStyleEditor(&s_winData);
    if(s_winData.showAppSimpleOverlay) showAppSimpleOverlay(&s_winData);
    if(s_winData.showAbout) showAbout(&s_winData);
    if (s_winData.evaluateClick) handleClickInViewport(&s_winData);
    if (s_winData.updateNodes) {updateEdges(&s_winData); updateNodes(&s_winData); }
    if (s_winData.showNodeDebugList) showDebugNodeList(&s_winData, [&](){
        if(s_targetEditable) showNodeEditor(&s_winData, s_targetEditable, true);
    });
    if (s_winData.showDebugLogs) showDebugLogs(&s_winData);
    
    ImGuiIO& io = ImGui::GetIO();
    if(io.KeyCtrl) {
        s_winData.multiSelectionEnabled = true;
    }
    if(s_selectedNodes.size() > 0 && s_winData.multiSelectionEnabled && !io.KeyCtrl){
        s_winData.keepMultiSelectionOpened = true;
        if(!s_tempPositionCaptured){
            s_tempPosition = {s_selectedNodes[0]->position.x, s_selectedNodes[0]->position.y};
            s_tempPositionCaptured = true;
        }
    }

    if(s_winData.multiSelectionEnabled){
        for(auto& n : s_selectedNodes){
            drawNode(*n, Config::s_selectionColor, true);
            n->print();
        }
    }else{
        s_selectedNodes.clear();
    }

    showPanel(&s_winData);
}
