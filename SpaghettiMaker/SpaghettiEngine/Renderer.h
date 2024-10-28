#pragma once
#include "types.h"
#include <GL/glew.h>
#include <memory>

class Renderer {
private:
    static Renderer* _instance;

    // OpenGL state
    bool _wireframeMode = false;
    bool _depthTestEnabled = true;
    bool _cullFaceEnabled = true;
    bool _lightingEnabled = true;

    Renderer() = default;

public:
    static Renderer* GetInstance() {
        if (!_instance) _instance = new Renderer();
        return _instance;
    }

    void Initialize();
    void Cleanup();

    // Frame functions
    void BeginFrame();
    void EndFrame();

    // State management
    void SetWireframeMode(bool enable);
    void SetDepthTest(bool enable);
    void SetCullFace(bool enable);
    void SetLighting(bool enable);

    // Getters
    bool IsWireframeModeEnabled() const { return _wireframeMode; }
    bool IsDepthTestEnabled() const { return _depthTestEnabled; }
    bool IsCullFaceEnabled() const { return _cullFaceEnabled; }
    bool IsLightingEnabled() const { return _lightingEnabled; }

    // Editor GUI
    void OnInspectorGUI();
};