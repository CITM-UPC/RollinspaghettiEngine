#include "MaterialComponent.h"
#include "imgui.h"
#include <glm/gtc/type_ptr.hpp>

void MaterialComponent::OnStart() {
    // Create shared checker texture if it doesn't exist
    if (!_diffuseMap) {
        _diffuseMap = TEXTURE_MANAGER->GetDefaultTexture();
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
    float shininess = static_cast<float>(_shininess * 128.0); // Convert to OpenGL's 0-128 range

    // Set material properties
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // Handle texturing
    glEnable(GL_TEXTURE_2D);

    if (_useCheckerTexture) {
        auto defaultTex = TEXTURE_MANAGER->GetDefaultTexture();
        if (defaultTex) {
            defaultTex->Bind(0);
        }
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
        // Color properties
        float ambient[3] = { static_cast<float>(_ambient.x), static_cast<float>(_ambient.y),
                           static_cast<float>(_ambient.z) };
        if (ImGui::ColorEdit3("Ambient", ambient)) {
            _ambient = vec3(ambient[0], ambient[1], ambient[2]);
        }

        float diffuse[3] = { static_cast<float>(_diffuse.x), static_cast<float>(_diffuse.y),
                            static_cast<float>(_diffuse.z) };
        if (ImGui::ColorEdit3("Diffuse", diffuse)) {
            _diffuse = vec3(diffuse[0], diffuse[1], diffuse[2]);
        }

        float specular[3] = { static_cast<float>(_specular.x), static_cast<float>(_specular.y),
                             static_cast<float>(_specular.z) };
        if (ImGui::ColorEdit3("Specular", specular)) {
            _specular = vec3(specular[0], specular[1], specular[2]);
        }

        float shininess = static_cast<float>(_shininess);
        if (ImGui::SliderFloat("Shininess", &shininess, 0.0f, 1.0f)) {
            _shininess = static_cast<double>(shininess);
        }

        // Texture settings
        ImGui::Separator();
        ImGui::Text("Texture Settings");

        if (_diffuseMap) {
            ImGui::Text("Current Texture: %s", _texturePath.c_str());
            ImGui::Text("Size: %dx%d", _diffuseMap->GetWidth(), _diffuseMap->GetHeight());

            // Texture preview
            ImGui::Separator();
            ImTextureID texId = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(_diffuseMap->GetID()));
            ImGui::Image(texId, ImVec2(100, 100));
        }
        else {
            ImGui::Text("No texture loaded");
        }

        // Checker texture toggle
        bool useChecker = _useCheckerTexture;
        if (ImGui::Checkbox("Use Checker Texture", &useChecker)) {
            SetUseCheckerTexture(useChecker);
        }

        ImGui::TextWrapped("Drag and drop PNG/DDS files here to apply texture");

        // Texture removal option
        if (!_useCheckerTexture && _diffuseMap && ImGui::Button("Remove Texture")) {
            _diffuseMap = TEXTURE_MANAGER->GetDefaultTexture();
            _texturePath.clear();
        }
    }
}