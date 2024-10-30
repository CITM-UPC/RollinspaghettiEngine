#include "Scene.h"
#include "imgui.h"
#include "ModelLoader.h"
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <GL/glew.h>
#include "Camera.h"



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

void Scene::Render() {
    if (!_root) return;

    // Save current OpenGL state
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // Set up basic OpenGL state for 3D rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Set up a basic light
    GLfloat lightPos[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    GLfloat lightAmb[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat lightDiff[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);

    // Render all game objects
    RenderGameObject(_root);

    // Restore OpenGL state
    glPopMatrix();
    glPopAttrib();
    
    // Backup of old rendering code

    //if (!_root) return;
    //
    //// Save OpenGL state
    //GLboolean lighting = glIsEnabled(GL_LIGHTING);
    //GLboolean texture2D = glIsEnabled(GL_TEXTURE_2D);

    //// Set up OpenGL state
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    //// Setup basic lighting
    //GLfloat lightPos[] = { 0.0f, 10.0f, 0.0f, 1.0f };
    //GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    //GLfloat lightDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };

    //glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    //glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    //glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);

    //// Render all GameObjects
    //for (const auto& gameObject : _gameObjects) {
    //    if (auto renderer = gameObject->GetComponent<RendererComponent>()) {
    //        renderer->OnUpdate();
    //    }
    //}
    //RenderGameObject(_root);
    //// Restore OpenGL state
    //if (!lighting) glDisable(GL_LIGHTING);
    //if (!texture2D) glDisable(GL_TEXTURE_2D);
}

void Scene::RenderGameObject(GameObject* gameObject) {
    if (!gameObject || !gameObject->IsActive()) return;

    glPushMatrix();

    // Apply transform if it exists
    if (auto transform = gameObject->GetComponent<TransformComponent>()) {
        glMultMatrixd(glm::value_ptr(transform->GetWorldMatrix()));
    }

    // Render mesh and material if they exist
    auto mesh = gameObject->GetComponent<MeshComponent>();
    auto material = gameObject->GetComponent<MaterialComponent>();

    if (mesh) {
        // The mesh component will handle its own rendering
        mesh->OnUpdate(); // This will trigger the mesh rendering
    }

    // Render all children
    for (const auto& child : gameObject->GetChildren()) {
        RenderGameObject(child.get());
    }

    glPopMatrix();
}

void Scene::FocusOnGameObject(GameObject* gameObject) {
    if (!gameObject || !_camera) return;

    auto transform = gameObject->GetComponent<TransformComponent>();
    if (!transform) return;

    // Get the position of the object
    vec3 objectPos = transform->GetWorldPosition();

    // Move camera to look at the object
    _camera->transform().pos() = objectPos + vec3(0, 2, 5); // Position camera above and behind
    _camera->lookAt(objectPos);
}

void Scene::HandleFileDrop(const char* path) {
    std::filesystem::path fsPath(path);
    std::string extension = fsPath.extension().string();

    // Convert to lowercase for comparison
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    if (extension == ".fbx") {
        // Load the model
        GameObject* loadedModel = ModelLoader::LoadModel(this, path);
        if (loadedModel) {
            std::cout << "Successfully loaded model: " << path << std::endl;
            // Auto-focus on the newly loaded model
            FocusOnGameObject(loadedModel);
            // Set it as selected object
            SetSelectedGameObject(loadedModel);
        }
        else {
            std::cerr << "Failed to load model: " << path << std::endl;
        }
    }
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