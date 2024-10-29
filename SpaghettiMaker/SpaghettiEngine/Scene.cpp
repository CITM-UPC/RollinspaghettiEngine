#include "Scene.h"
#include "imgui.h"
#include <algorithm>

Scene::Scene(const char* name) : _name(name) {
    // Create root GameObject
    _root = new GameObject("Scene Root");
}

Scene::~Scene() {
    Stop();
    delete _root;
}

void Scene::Start() {
    if (_isPlaying) return;

    _isPlaying = true;
    _isPaused = false;

    // Start all GameObjects
    for (auto& gameObject : _gameObjects) {
        for (const auto& component : gameObject->GetComponents()) {
            component->OnStart();
        }
    }
}

void Scene::Update() {
    if (!_isPlaying || _isPaused) return;

    // Update starting from root
    _root->Update();
}

void Scene::Stop() {
    if (!_isPlaying) return;

    _isPlaying = false;
    _isPaused = false;

    // Clean up any play mode state if needed
}

void Scene::Pause(bool pause) {
    if (!_isPlaying) return;
    _isPaused = pause;
}

GameObject* Scene::CreateGameObject(const char* name, GameObject* parent) {
    auto gameObject = std::make_shared<GameObject>(name);

    // Set parent (use root if none specified)
    gameObject->SetParent(parent ? parent : _root);

    // Add to scene's GameObject list
    _gameObjects.push_back(gameObject);

    return gameObject.get();
}

void Scene::DestroyGameObject(GameObject* gameObject) {
    if (!gameObject) return;

    // First, destroy all children
    auto children = gameObject->GetChildren();
    for (const auto& child : children) {
        DestroyGameObject(child.get());
    }

    // Remove from parent
    if (GameObject* parent = gameObject->GetParent()) {
        gameObject->SetParent(nullptr);
    }

    // Remove from scene's GameObject list
    _gameObjects.erase(
        std::remove_if(_gameObjects.begin(), _gameObjects.end(),
            [gameObject](const auto& go) { return go.get() == gameObject; }),
        _gameObjects.end());
}

void Scene::OnHierarchyGUI() {
    ImGui::Begin("Hierarchy");

    // Scene controls
    if (ImGui::Button(_isPlaying ? "Stop" : "Play")) {
        if (_isPlaying) Stop();
        else Start();
    }

    ImGui::SameLine();

    if (_isPlaying) {
        if (ImGui::Button(_isPaused ? "Resume" : "Pause")) {
            Pause(!_isPaused);
        }
    }

    ImGui::Separator();

    // Add GameObject button
    if (ImGui::Button("Add GameObject")) {
        CreateGameObject("New GameObject");
    }

    ImGui::Separator();

    // Draw hierarchy tree starting from root
    DrawHierarchyNode(_root);

    ImGui::End();
}

void Scene::DrawHierarchyNode(GameObject* node) {
    if (!node) return;

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    // Add selected flag if this is the selected GameObject
    if (node == _selectedGameObject) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    // Don't show the root node
    if (node == _root) {
        // Directly draw children of root
        for (const auto& child : node->GetChildren()) {
            DrawHierarchyNode(child.get());
        }
        return;
    }

    // Regular node display
    const auto& children = node->GetChildren();
    bool hasChildren = !children.empty();

    // If no children, use Bullet flags to show as a leaf node
    if (!hasChildren) {
        flags |= ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_Leaf;
    }

    bool nodeOpen = ImGui::TreeNodeEx(node->GetName().c_str(), flags);

    // Handle selection
    if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
        SetSelectedGameObject(node);
    }

    // Context menu
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Add Child")) {
            CreateGameObject("New GameObject", node);
        }
        if (ImGui::MenuItem("Delete")) {
            DestroyGameObject(node);
            if (node == _selectedGameObject) {
                SetSelectedGameObject(nullptr);
            }
        }
        ImGui::EndPopup();
    }

    // Draw children if node is open
    if (nodeOpen) {
        for (const auto& child : children) {
            DrawHierarchyNode(child.get());
        }
        ImGui::TreePop();
    }
}

void Scene::OnInspectorGUI() {
    ImGui::Begin("Inspector");

    if (_selectedGameObject) {
        // GameObject name
        char nameBuf[256];
        strncpy_s(nameBuf, _selectedGameObject->GetName().c_str(), sizeof(nameBuf) - 1);
        if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
            // _selectedGameObject->SetName(nameBuf);  // You'll need to add this method to GameObject
        }

        ImGui::Separator();

        // Display components
        for (const auto& component : _selectedGameObject->GetComponents()) {
            if (ImGui::CollapsingHeader(component->GetName().c_str())) {
                component->OnInspectorGUI();
            }
        }

        // Add Component button
        if (ImGui::Button("Add Component")) {
            // TODO: Show component selection popup
            // This will be implemented when we add more component types
        }
    }
    else {
        ImGui::Text("No GameObject selected");
    }

    ImGui::End();
}