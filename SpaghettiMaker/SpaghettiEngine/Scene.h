#pragma once
#include "GameObject.h"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include "RendererComponent.h"


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

    void HandleFileDrop(const char* path);

    void Render() {
        // Save OpenGL state
        GLboolean lighting = glIsEnabled(GL_LIGHTING);
        GLboolean texture2D = glIsEnabled(GL_TEXTURE_2D);

        // Set up OpenGL state
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        // Setup basic lighting
        GLfloat lightPos[] = { 0.0f, 10.0f, 0.0f, 1.0f };
        GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
        GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

        // Render all GameObjects
        for (const auto& gameObject : _gameObjects) {
            if (auto renderer = gameObject->GetComponent<RendererComponent>()) {
                renderer->OnUpdate();
            }
        }

        // Restore OpenGL state
        if (!lighting) glDisable(GL_LIGHTING);
        if (!texture2D) glDisable(GL_TEXTURE_2D);
    }

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