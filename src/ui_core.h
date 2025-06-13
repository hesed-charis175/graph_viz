#include "imgui.h"
#include "config.h"
#include <iostream>
#include "namespace.h"
#include <string>
#include <chrono>
#include <deque>
#include <vector>


struct KeyBufferEntry{
    ImGuiKey key;
    std::string name;
    std::chrono::steady_clock::time_point timestamp;
};


static std::deque<KeyBufferEntry> s_KeyBuffer;
static constexpr float s_multiKeyDuration = 0.3f;
static constexpr float s_uniqueKeyDuration = 1.0f;

static bool s_CopiedOnceFlag = false;
static bool s_WasPopupOpen = false;

static std::vector<Node*> s_selectedNodes;

Node s_localGrStart, s_localGrEnd;


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

#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))

static bool s_configOpen = NULL;

static Node s_copiedNode;
static Node* s_targetEditable = nullptr;

struct WindowData{
    bool showMainMenuBar = true;
    bool showAppSimpleOverlay = true;
    bool showMetrics = false;
    bool showAbout = false;
    bool showStyleEditor = false;
    bool showNodeEditor = false;
    bool evaluateClick = true;
    bool updateNodes = true;
    bool editableNode = false;
};
