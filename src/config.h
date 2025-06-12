#pragma once

#include "imgui.h"

namespace Config{

    const char* version = "1.0.0 unique";
    int windowWidth = 1280;
    int windowHeight = 720;
    float zoomFactor = 1.0f;

    float zoomMax = 2.0f, zoomMin = 0.5f;

    int nodeSize = 10.0f;
    int maxNodeSize = 30.0f, minNodeSize = 5.0f;
    int edgeThickness = 3.0f;

    float minGlowFactor = 0.0f;
    float glowFactor = 1.0f;
    float maxGlowFactor = 2.0f;

    int glowIterations = 5;

    bool enableDebugTools = true;


    const ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 blue = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
    const ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

    const ImVec4 black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    const ImVec4 orange = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    const ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

    const ImVec4 darkTheme = ImVec4(0.0901961f, 0.101961f, 0.109804f, 1.00f); 
    const ImVec4 blueDarkTheme = ImVec4(0.187212f, 0.219003f, 0.240196f, 1.0f);
    const ImVec4 redDarkTheme = ImVec4(0.112745f, 0.0110534f, 0.0110534f, 1.0f);
    // const ImVec4 lightTheme = ImVec4();

    ImVec4 s_viewportColor  = darkTheme;

    ImVec4 s_edgeColor = white;
    ImVec4 s_nodeColor = blue;
    ImVec4 s_startColor = green;
    ImVec4 s_endColor = red;
    ImVec4 s_intermediateColor = orange;
    ImVec4 s_pathColor = yellow;
    ImVec4 s_auxPathColor = black;

    bool showPerformaceMetrics = true;
    bool allowNodeDragging = true;
    bool displayPosition = true;

    void setZoom(float zoom){
        zoomFactor = zoom;
    }

    void AdjustZoom(){
        if (zoomFactor < 0.5f) zoomFactor = 0.5f;
        if (zoomFactor > 2.0f) zoomFactor = 2.0f;

    }
}


namespace Default_Strings{
    const char* colorHelpMarker = "Click on the color square to open a color picker\n"
            "Click and hold to use drag and drop.\n"
            "Right-click on the color sqaure to show options.\n"
            "CTRL+click on individual component to input value";
    const char* nodeHelpMarker = "Select a node type.\n\n"
        "Start is the start point of the search - It holds only for one node\n\n"
        "Intermediate represents nodes that must be passed through by the algorithm.\n\n"
        "End is the goal of the search - It holds only for one node\n\n";
}
