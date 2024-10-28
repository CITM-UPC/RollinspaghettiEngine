#pragma once
#include "Component.h"
#include "types.h"
#include <vector>
#include <memory>
#include <glm/vec2.hpp> // Include GLM vec2
#include <glm/vec3.hpp> // Include GLM vec3


struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texCoords;

    Vertex(const vec3& pos = vec3(0), const vec3& norm = vec3(0), const vec2& tex = vec2(0))
        : position(pos), normal(norm), texCoords(tex) {}
};

class MeshComponent : public Component {
private:
    std::vector<Vertex> _vertices;
    std::vector<unsigned int> _indices;

    // OpenGL buffer objects
    unsigned int _vao = 0; // Vertex Array Object
    unsigned int _vbo = 0; // Vertex Buffer Object
    unsigned int _ebo = 0; // Element Buffer Object

    bool _showNormals = false;
    float _normalLength = 0.1f; // Length of normal visualization lines

    void SetupMesh();
    void CleanupMesh();

public:
    MeshComponent() : Component("Mesh") {}
    ~MeshComponent();

    // Component interface
    void OnStart() override;
    void OnDestroy() override;
    void OnUpdate() override;
    void OnInspectorGUI() override;

    // Mesh data management
    void SetMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Debug visualization
    void SetShowNormals(bool show) { _showNormals = show; }
    bool GetShowNormals() const { return _showNormals; }
    void SetNormalLength(float length) { _normalLength = length; }

    // Getters for mesh data
    const std::vector<Vertex>& GetVertices() const { return _vertices; }
    const std::vector<unsigned int>& GetIndices() const { return _indices; }
    unsigned int GetVAO() const { return _vao; }
};