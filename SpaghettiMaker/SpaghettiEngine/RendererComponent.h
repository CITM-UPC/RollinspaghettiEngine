#pragma once
#include "Component.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_opengl3.h>

class RendererComponent : public Component {
private:
    std::shared_ptr<MeshComponent> _meshComponent = nullptr;
    std::shared_ptr<MaterialComponent> _materialComponent = nullptr;
    bool _isVisible = true;

public:
    RendererComponent() : Component("Renderer") {}

    void OnStart() override {
        // Get required components
        _meshComponent = GetOwner()->GetComponent<MeshComponent>();
        _materialComponent = GetOwner()->GetComponent<MaterialComponent>();
    }

    void OnUpdate() override {
        if (!_isVisible || !_meshComponent || !_materialComponent) return;

        // Get transform matrix from owner's transform component
        if (auto transform = GetOwner()->GetComponent<TransformComponent>()) {
            glPushMatrix();
            glMultMatrixd(glm::value_ptr(transform->GetWorldMatrix()));
        }

        // Set material properties
        if (_materialComponent) {
            _materialComponent->OnUpdate();
        }

        // Draw mesh
        if (_meshComponent) {
            _meshComponent->OnUpdate();
        }

        glPopMatrix();
    }

    void OnInspectorGUI() override {
        ImGui::Checkbox("Visible", &_isVisible);
    }

    void SetVisible(bool visible) { _isVisible = visible; }
    bool IsVisible() const { return _isVisible; }
};