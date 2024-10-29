#pragma once
#include "Component.h"
#include "Texture.h"
#include "TextureManager.h"
#include "types.h"

class MaterialComponent : public Component {
private:
    // Material properties
    vec3 _ambient = vec3(0.2);
    vec3 _diffuse = vec3(0.8);
    vec3 _specular = vec3(0.0);
    double _shininess = 0.0;

    // Textures
    TexturePtr _diffuseMap;
    bool _useCheckerTexture = false;
    std::string _texturePath;

public:
    MaterialComponent() : Component("Material") {
        _diffuseMap = TEXTURE_MANAGER->GetDefaultTexture();
    }

    // Component interface
    //void OnStart() override;
    //void OnUpdate() override;
    //void OnInspectorGUI() override;

    // Material property setters
    void SetAmbient(const vec3& color) { _ambient = color; }
    void SetDiffuse(const vec3& color) { _diffuse = color; }
    void SetSpecular(const vec3& color) { _specular = color; }
    void SetShininess(double value) { _shininess = value; }

    // Updated texture management
    bool SetDiffuseTexture(const std::string& path) {
        _diffuseMap = TEXTURE_MANAGER->LoadTexture(path);
        if (_diffuseMap) {
            _texturePath = path;
            _useCheckerTexture = false;
            return true;
        }
        return false;
    }

    void SetUseCheckerTexture(bool use) {
        _useCheckerTexture = use;
        if (use) {
            _diffuseMap = TEXTURE_MANAGER->GetDefaultTexture();
            _texturePath.clear();
        }
    }

    // Property getters
    const vec3& GetAmbient() const { return _ambient; }
    const vec3& GetDiffuse() const { return _diffuse; }
    const vec3& GetSpecular() const { return _specular; }
    double GetShininess() const { return _shininess; }
    TexturePtr GetDiffuseTexture() const { return _diffuseMap; }
    bool IsUsingCheckerTexture() const { return _useCheckerTexture; }
};