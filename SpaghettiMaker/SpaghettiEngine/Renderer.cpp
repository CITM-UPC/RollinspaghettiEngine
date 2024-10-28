// SpaghettiEngine/Graphics/Renderer.cpp
#include "Renderer.h"
#include "imgui.h"

Renderer* Renderer::_instance = nullptr;

void Renderer::Initialize() {
    // Initialize OpenGL states
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Set default render states
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClearDepth(1.0f);

    // Set default material properties
    GLfloat defaultAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat defaultDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat defaultSpecular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
    GLfloat defaultShininess = 0.0f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, defaultAmbient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, defaultDiffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, defaultSpecular);
    glMaterialf(GL_FRONT, GL_SHININESS, defaultShininess);
}

void Renderer::Cleanup() {
    // Cleanup renderer resources if needed
}

void Renderer::BeginFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::EndFrame() {
    // Any end-of-frame operations
}

void Renderer::SetWireframeMode(bool enable) {
    _wireframeMode = enable;
    glPolygonMode(GL_FRONT_AND_BACK, enable ? GL_LINE : GL_FILL);
}

void Renderer::SetDepthTest(bool enable) {
    _depthTestEnabled = enable;
    if (enable) glEnable(GL_DEPTH_TEST);
    else glDisable(GL_DEPTH_TEST);
}

void Renderer::SetCullFace(bool enable) {
    _cullFaceEnabled = enable;
    if (enable) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
}

void Renderer::SetLighting(bool enable) {
    _lightingEnabled = enable;
    if (enable) glEnable(GL_LIGHTING);
    else glDisable(GL_LIGHTING);
}

void Renderer::OnInspectorGUI() {
    if (ImGui::CollapsingHeader("Renderer Settings")) {
        bool wireframe = IsWireframeModeEnabled();
        if (ImGui::Checkbox("Wireframe Mode", &wireframe)) {
            SetWireframeMode(wireframe);
        }

        bool depthTest = IsDepthTestEnabled();
        if (ImGui::Checkbox("Depth Test", &depthTest)) {
            SetDepthTest(depthTest);
        }

        bool cullFace = IsCullFaceEnabled();
        if (ImGui::Checkbox("Face Culling", &cullFace)) {
            SetCullFace(cullFace);
        }

        bool lighting = IsLightingEnabled();
        if (ImGui::Checkbox("Lighting", &lighting)) {
            SetLighting(lighting);
        }
    }
}