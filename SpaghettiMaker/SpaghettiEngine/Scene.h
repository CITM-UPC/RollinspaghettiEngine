#pragma once
#include "GameObject.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

class Scene {
private:
    std::string _name;
    GameObject* _root;  // Root GameObject that holds the scene hierarchy
    std::vector<std::shared_ptr<GameObject>> _gameObjects; // All GameObjects in scene
    GameObject* _selectedGameObject = nullptr;

    // Track active state
    bool _isPlaying = false;
    bool _isPaused = false;

public:
    Scene(const char* name = "New Scene");
    ~Scene();

    // Scene lifecycle
    void Start();
    void Update();
    void Stop();
    void Pause(bool pause);

    // GameObject management
    GameObject* CreateGameObject(const char* name = "GameObject", GameObject* parent = nullptr);
    void DestroyGameObject(GameObject* gameObject);

    // Scene hierarchy
    GameObject* GetRoot() const { return _root; }
    const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const { return _gameObjects; }

    // Selection handling
    void SetSelectedGameObject(GameObject* gameObject) { _selectedGameObject = gameObject; }
    GameObject* GetSelectedGameObject() const { return _selectedGameObject; }

    // State queries
    bool IsPlaying() const { return _isPlaying; }
    bool IsPaused() const { return _isPaused; }
    const std::string& GetName() const { return _name; }

    // Editor GUI
    void OnHierarchyGUI();
    void OnInspectorGUI();

private:
    void DrawHierarchyNode(GameObject* node);
    void CleanupGameObject(GameObject* gameObject);
};