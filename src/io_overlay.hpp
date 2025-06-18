#include "render_core.hpp"


// An IO debugging overlay

struct KeyBufferEntry{
    ImGuiKey key;
    std::string name;
    std::chrono::steady_clock::time_point timestamp;
};


static std::deque<KeyBufferEntry> s_KeyBuffer;
static constexpr float s_multiKeyDuration = 0.3f;
static constexpr float s_uniqueKeyDuration = 1.0f;

static void updateKeyBuffer(){
    auto now = std::chrono::steady_clock::now();

    while(!s_KeyBuffer.empty()){
        float elapsed = std::chrono::duration<float>(now - s_KeyBuffer.front().timestamp).count();
        if(s_KeyBuffer.size() == 1 && elapsed >= s_uniqueKeyDuration){
            s_KeyBuffer.pop_front();
        }
        else if(s_KeyBuffer.size() > 1 && elapsed >= s_multiKeyDuration){
            s_KeyBuffer.pop_front();
        }else{
            break;
        }
    }
}

static void showDebugLogs(WindowData* winData) {
    static int location = 2;
    static std::unordered_set<std::string> uniqueLogs;
    static std::vector<std::string> logHistory;
    static bool scrollToBottom = false;

    if (!s_toLog.empty() && uniqueLogs.find(s_toLog) == uniqueLogs.end()) {
        uniqueLogs.insert(s_toLog);
        logHistory.push_back(s_toLog);
        scrollToBottom = true;
    }

    ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoDecoration | 
                                 ImGuiWindowFlags_NoDocking | 
                                 ImGuiWindowFlags_NoSavedSettings | 
                                 ImGuiWindowFlags_NoFocusOnAppearing | 
                                 ImGuiWindowFlags_NoNav | 
                                 ImGuiWindowFlags_NoCollapse | 
                                 ImGuiWindowFlags_NoMove;

    const float PAD = 10.0f;
    const float fixedWidth = 600.0f;
    const float fixedHeight = 500.0f;

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
    ImGui::SetNextWindowSize(ImVec2(fixedWidth, fixedHeight));
    ImGui::SetNextWindowBgAlpha(0.35f);

    if (ImGui::Begin("Graph Memory Logs", &winData->showAppSimpleOverlay, winFlags)) {
        ImGui::Text("Graph Memory Logs");
        ImGui::Separator();

        ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        
        for (const std::string& log : logHistory) {
            ImGui::TextWrapped("%s", log.c_str());
            std::cout << log << std::endl;
        }

        if (scrollToBottom) {
            ImGui::SetScrollHereY(1.0f);
            scrollToBottom = false;
        }

        ImGui::EndChild();
    }

    ImGui::End();
}

enum class DebugPosition {TopLeft, TopRight, BottomLeft, BottomRight};
static void showDebugNodeList(WindowData* winData){
    static int location = 1;

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

    ImVec2 minSize(0, 0);
    ImVec2 maxSize(400, 300);
    ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

    if (ImGui::Begin("Node Debug List", &winData->showAppSimpleOverlay, winFlags))
    {
        ImGui::Text("Node Debug List");
        ImGui::Separator();

        for(auto node : s_Graph.nodes){
            bool isBeingEdited = false;
            if(winData->editableNode && &node == s_targetEditable) isBeingEdited = true;
            ImGui::Text("[NODE_ID %d] : Position(%.2f, %.2f) | Type(%s)", static_cast<int>(node.node_id), node.position.x, node.position.y, node.debugNodeTypeString.c_str());
            if(isBeingEdited) {
                ImGui::SameLine();
                ImGui::Text("<Edit Mode>");
            }
            ImGui::Separator();
        }

        if(winData->editableNode){
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("<Edit Mode>");
        }
    }
    ImGui::End();

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
