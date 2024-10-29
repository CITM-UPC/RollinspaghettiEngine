// SpaghettiEngine/Core/GameObject.h
#pragma once
#include "Component.h"
#include <vector>
#include <memory>
#include <string>
#include <typeinfo>
#include <algorithm>
#include "Renderer.h"

class GameObject {
private:
    bool _active = true;
    std::string _name;
    std::vector<ComponentPtr> _components;

    // Scene hierarchy
    GameObject* _parent = nullptr;
    std::vector<std::shared_ptr<GameObject>> _children;

public:
    GameObject(const char* name = "GameObject") : _name(name) {}
    ~GameObject() {
        // Cleanup components
        for (auto& component : _components) {
            component->OnDestroy();
        }
        _components.clear();
    }

    // Component management
    template<typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args) {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        component->_owner = this;
        component->OnStart();
        _components.push_back(component);

        return std::static_pointer_cast<T>(component);
    }

    template<typename T>
    std::shared_ptr<T> GetComponent() {
        static_assert(std::is_base_of<Component, T>::value, "T must inherit from Component");

        for (auto& component : _components) {
            if (auto derived = std::dynamic_pointer_cast<T>(component)) {
                return derived;
            }
        }
        return nullptr;
    }

    // Hierarchy management
    void SetParent(GameObject* parent) {
        if (_parent == parent) return;
        if (_parent) {
            // Remove from old parent
            auto& siblings = _parent->_children;
            siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
                [this](const auto& child) { return child.get() == this; }),
                siblings.end());
        }
        _parent = parent;
        if (_parent) {
            _parent->_children.push_back(std::shared_ptr<GameObject>(this));
        }
    }

    // Lifecycle
    virtual void Update() {
        if (!_active) return;

        // Update components
        for (auto& component : _components) {
            if (component->IsActive()) {
                component->OnUpdate();
            }
        }

        // Update children
        for (auto& child : _children) {
            child->Update();
        }
    }

    // State management
    bool IsActive() const { return _active; }
    void SetActive(bool active) {
        if (_active != active) {
            _active = active;
            // Propagate to components
            for (auto& component : _components) {
                if (active) component->OnEnable();
                else component->OnDisable();
            }
        }
    }

	void paint();

    // Getters
    const std::string& GetName() const { return _name; }
    GameObject* GetParent() const { return _parent; }
    const std::vector<std::shared_ptr<GameObject>>& GetChildren() const { return _children; }
    const std::vector<ComponentPtr>& GetComponents() const { return _components; }
};