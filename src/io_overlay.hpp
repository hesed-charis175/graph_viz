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
