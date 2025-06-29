static void showGrid(WindowData* winData){
    ImDrawList* draw_list = ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
    ImVec2 canvasSize = ImGui::GetMainViewport()->Size;
    ImVec2 mousePos = lastZoomFocus;
    float buffer = Config::maxPan / Config::zoom;

    ImVec2 origin = {viewportPos.x + Config::panOffset.x, viewportPos.y + Config::panOffset.y};
    float spacing = Config::gridSpacing * Config::zoom;

    float margin = 60.0f;

    ImVec2 gridStart = ImVec2(origin.x, origin.y);
    ImVec2 gridEnd = ImVec2(origin.x + canvasSize.x, origin.y + canvasSize.y);

    float xOffset = fmod(origin.x, spacing);
    float yOffset = fmod(origin.y, spacing);

    for (float x = gridStart.x; x < gridEnd.x; x += spacing)
    {
        draw_list->AddLine(ImVec2(x, gridStart.y), ImVec2(x, gridEnd.y), IM_COL32(200, 200, 200, 40));
    }

    for (float y = gridStart.y; y < gridEnd.y; y += spacing)
    {
        draw_list->AddLine(ImVec2(gridStart.x, y), ImVec2(gridEnd.x, y), IM_COL32(200, 200, 200, 40));
    }

}

static ImVec2 joystickCenter;
static bool dragging = false;
static ImVec2 joystickOffset = ImVec2(0, 0);

ImVec2 ImLerp(const ImVec2& a, const ImVec2& b, float t) {
    return ImVec2(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t
    );
}
float ImLengthSqr(const ImVec2& v) {
    return v.x * v.x + v.y * v.y;
}

ImVec2 ImVec2Normalize(const ImVec2& v) {
    float length = sqrtf(v.x * v.x + v.y * v.y);
    if (length == 0.0f)
        return ImVec2(0, 0);
    return ImVec2(v.x / length, v.y / length);
}

float ImClamp(float *v, float min, float max) {
    if(*v > max) *v = max;
    else if(*v < min) *v = min;
    return *v;
}

void showCircularJoystick(WindowData* winData) {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImDrawList* drawList = ImGui::GetBackgroundDrawList(viewport);

    float PAD = 90.0f;
    float radius = Config::joystickRadius;
    ImVec2 basePosition = ImVec2(viewport->Pos.x + viewport->Size.x - PAD, viewport->Pos.y + viewport->Size.y - PAD);

    joystickCenter = basePosition;

    ImVec2 mousePosition = ImGui::GetIO().MousePos;
    bool mouseDown = ImGui::IsMouseDown(0);

    float distanceSquared = (mousePosition.x - joystickCenter.x) * (mousePosition.x - joystickCenter.x)
                       + (mousePosition.y - joystickCenter.y) * (mousePosition.y - joystickCenter.y);
    
    winData->hoveringJoystick = distanceSquared <= (radius * radius);

    if (!dragging && ImLengthSqr({mousePosition.x - joystickCenter.x, 
        mousePosition.y - joystickCenter.y}) < radius * radius && mouseDown) {
        dragging = true;
    }
    if (!mouseDown)
        dragging = false;

    ImVec2 joystickDelta = ImVec2(0, 0);

    float speed = 3.0f;

    float maxOffset = radius;

    ImVec2 normalizedOffset = {joystickOffset.x / maxOffset, joystickOffset.y / maxOffset};

    normalizedOffset.x = ImClamp(&normalizedOffset.x, -1.0f, 1.0f);
    normalizedOffset.y = ImClamp(&normalizedOffset.y, -1.0f, 1.0f);

    joystickDelta = {
        normalizedOffset.x * speed,
        normalizedOffset.y * speed
    };

    if (dragging) {
        joystickOffset = {mousePosition.x - joystickCenter.x, mousePosition.y - joystickCenter.y};
        float dist = ImLengthSqr(joystickOffset);
        if (dist > radius * radius) joystickOffset = {ImVec2Normalize(joystickOffset).x * radius, ImVec2Normalize(joystickOffset).y * radius};
        
        Config::panOffset.x += joystickDelta.x;
        Config::panOffset.y += joystickDelta.y;

        Config::panOffset.x = ImClamp(&Config::panOffset.x, -Config::maxPan, Config::maxPan);
        Config::panOffset.y = ImClamp(&Config::panOffset.y, -Config::maxPan, Config::maxPan);

    } 
    else { joystickOffset = ImLerp(joystickOffset, ImVec2(0, 0), 0.2f); }

    drawList->AddCircleFilled(joystickCenter, radius, IM_COL32(100, 100, 100, 100));
    drawList->AddCircleFilled({joystickCenter.x + joystickOffset.x, joystickCenter.y + joystickOffset.y}, radius * 0.4f, IM_COL32(200, 200, 255, 180));
}

void scrollToZoom(){
    ImGuiIO& io = ImGui::GetIO();
    float scroll = io.MouseWheel;
    if(scroll != 0.0f){
        lastZoomFocus = ImGui::GetIO().MousePos;
        lastZoom = Config::zoom;
        ImVec2 mousePosition = io.MousePos;

        float prevZoom = Config::zoom;
        Config::zoom *= (1.0f + scroll * 0.1f);
        Config::zoom = ImClamp(&Config::zoom, 0.5f, 1.5f);

        float scale = Config::zoom / prevZoom;
        Config::panOffset.x = mousePosition.x - (mousePosition.x - Config::panOffset.x) * scale;
        Config::panOffset.y = mousePosition.y - (mousePosition.y - Config::panOffset.y) * scale;
    }
}
