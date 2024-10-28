// SpaghettiEngine/Graphics/MaterialComponent.cpp
#include "MaterialComponent.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

TexturePtr MaterialComponent::s_checkerTexture = nullptr;

void MaterialComponent::OnStart() {
    if (!s_checkerTexture) {
        s_checkerTexture = Texture::CreateCheckerboard();
    }
}

void MaterialComponent::OnUpdate() {
    // Convert double vectors to float arrays for OpenGL
    float ambient[4] = { static_cast<float>(_ambient.x), static_cast<float>(_ambient.y),
                        static_cast<float>(_ambient.z), 1.0f };
    float diffuse[4] = { static_cast<float>(_diffuse.x), static_cast<float>(_diffuse.y),
                         static_cast<float>(_diffuse.z), 1.0f };
    float specular[4] = { static_cast<float>(_specular.x), static_cast<float>(_specular.y),
                          static_cast<float>(_specular.z), 1.0f };
    float shininess = static_cast<float>(_shininess * 128.0);

    // Set material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // Handle texturing
    if (_useCheckerTexture && s_checkerTexture) {
        s_checkerTexture->Bind(0);
    }
    else if (_diffuseMap) {
        _diffuseMap->Bind(0);
    }
    else {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void MaterialComponent::OnInspectorGUI() {
    if (ImGui::CollapsingHeader("Material Properties")) {
        // Convert double vectors to float vectors for ImGui
        float ambientf[3] = { static_cast<float>(_ambient.x), static_cast<float>(_ambient.y),
                             static_cast<float>(_ambient.z) };
        float diffusef[3] = { static_cast<float>(_diffuse.x), static_cast<float>(_diffuse.y),
                             static_cast<float>(_diffuse.z) };
        float specularf[3] = { static_cast<float>(_specular.x), static_cast<float>(_specular.y),
                              static_cast<float>(_specular.z) };
        float shininess = static_cast<float>(_shininess);

        // Color properties
        if (ImGui::ColorEdit3("Ambient", ambientf)) {
            SetAmbient(vec3(ambientf[0], ambientf[1], ambientf[2]));
        }

        if (ImGui::ColorEdit3("Diffuse", diffusef)) {
            SetDiffuse(vec3(diffusef[0], diffusef[1], diffusef[2]));
        }

        if (ImGui::ColorEdit3("Specular", specularf)) {
            SetSpecular(vec3(specularf[0], specularf[1], specularf[2]));
        }

        if (ImGui::SliderFloat("Shininess", &shininess, 0.0f, 1.0f)) {
            SetShininess(static_cast<double>(shininess));
        }

        // Texture options
        ImGui::Separator();
        ImGui::Text("Texture Settings");

        if (_diffuseMap) {
            ImGui::Text("Current Texture: %s", _diffuseMap->GetPath().c_str());
            ImGui::Text("Size: %dx%d", _diffuseMap->GetWidth(), _diffuseMap->GetHeight());
        }

        bool useChecker = _useCheckerTexture;
        if (ImGui::Checkbox("Use Checker Texture", &useChecker)) {
            SetUseCheckerTexture(useChecker);
        }
    }
}