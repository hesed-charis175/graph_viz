#include "imgui.h"
#include "config.h"
#include <iostream>
#include "global.h"
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


static std::vector<Node*> s_selectedNodes;

#define IM_MIN(A, B)            (((A) < (B)) ? (A) : (B))
static bool s_configOpen = NULL;

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

static Node s_copiedNode;
