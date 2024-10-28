// SpaghettiEngine/Core/Component.h
#pragma once
#include <string>
#include <memory>

class GameObject;

class Component {
protected:
    bool _active = true;
    GameObject* _owner = nullptr;
    std::string _name;

public:
    Component(const char* name = "Component") : _name(name) {}
    virtual ~Component() = default;

    // Core component interface
    virtual void OnStart() {} // Called when component is first created
    virtual void OnEnable() {} // Called when component or GameObject is enabled
    virtual void OnDisable() {} // Called when component or GameObject is disabled
    virtual void OnUpdate() {} // Called every frame when active
    virtual void OnDestroy() {} // Called when component is being destroyed

    // Optional editor interface
    virtual void OnEditorUpdate() {} // Called every frame in editor
    virtual void OnInspectorGUI() {} // Called when component is selected in inspector

    // Getters/Setters
    bool IsActive() const { return _active && _owner && _owner->IsActive(); }
    void SetActive(bool active) {
        if (_active != active) {
            _active = active;
            if (_active) OnEnable();
            else OnDisable();
        }
    }

    const std::string& GetName() const { return _name; }
    GameObject* GetOwner() const { return _owner; }

    // Friend class to allow GameObject to set owner
    friend class GameObject;
};

// Common component type definitions
using ComponentPtr = std::shared_ptr<Component>;