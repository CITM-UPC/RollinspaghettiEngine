#pragma once
#include "Component.h"
#include "Texture.h"
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
    static TexturePtr s_checkerTexture; // Shared checker texture

public:
    MaterialComponent() : Component("Material") {}

    // Component interface
    void OnStart() override;
    void OnUpdate() override;
    void OnInspectorGUI() override;

    // Material property setters
    void SetAmbient(const vec3& color) { _ambient = color; }
    void SetDiffuse(const vec3& color) { _diffuse = color; }
    void SetSpecular(const vec3& color) { _specular = color; }
    void SetShininess(double value) { _shininess = value; }

    // Texture management
    bool SetDiffuseTexture(const std::string& path);
    void SetUseCheckerTexture(bool use);
    bool HasDiffuseTexture() const { return _diffuseMap != nullptr || _useCheckerTexture; }

    // Property getters
    const vec3& GetAmbient() const { return _ambient; }
    const vec3& GetDiffuse() const { return _diffuse; }
    const vec3& GetSpecular() const { return _specular; }
    double GetShininess() const { return _shininess; }
    TexturePtr GetDiffuseTexture() const { return _diffuseMap; }
    bool IsUsingCheckerTexture() const { return _useCheckerTexture; }
};