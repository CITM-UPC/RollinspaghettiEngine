// SpaghettiEngine/Graphics/MeshComponent.cpp
#include "MeshComponent.h"
#include "GameObject.h"
#include "TransformComponent.h"
#include <GL/glew.h>
#include "imgui.h"
#include <iostream>

MeshComponent::~MeshComponent() {
    CleanupMesh();
}

void MeshComponent::OnStart() {
    SetupMesh();
}

void MeshComponent::OnDestroy() {
    CleanupMesh();
}

void MeshComponent::SetupMesh() {
    if (_vertices.empty() || _indices.empty()) return;

    // Create and bind VAO first
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    // Create vertex buffer
    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    // Calculate stride and prepare vertex data
    const size_t stride = sizeof(Vertex);
    std::vector<float> vertexData;
    vertexData.reserve(_vertices.size() * 8); // 3 pos + 3 normal + 2 uv = 8 floats per vertex

    for (const auto& vertex : _vertices) {
        // Position
        vertexData.push_back(static_cast<float>(vertex.position.x));
        vertexData.push_back(static_cast<float>(vertex.position.y));
        vertexData.push_back(static_cast<float>(vertex.position.z));

        // Normal
        vertexData.push_back(static_cast<float>(vertex.normal.x));
        vertexData.push_back(static_cast<float>(vertex.normal.y));
        vertexData.push_back(static_cast<float>(vertex.normal.z));

        // UV coordinates
        vertexData.push_back(static_cast<float>(vertex.texCoords.x));
        vertexData.push_back(static_cast<float>(vertex.texCoords.y));
    }

    // Upload vertex data
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Create and set up index buffer
    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    // UV attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Unbind VAO to prevent accidental modifications
    glBindVertexArray(0);

    // Debug output
    std::cout << "Mesh buffer setup completed:" << std::endl;
    std::cout << "- VAO: " << _vao << std::endl;
    std::cout << "- VBO: " << _vbo << std::endl;
    std::cout << "- EBO: " << _ebo << std::endl;
    std::cout << "- Vertex count: " << _vertices.size() << std::endl;
    std::cout << "- Index count: " << _indices.size() << std::endl;

    // Debug first few vertices
    for (size_t i = 0; i < std::min(_vertices.size(), size_t(5)); ++i) {
        const auto& v = _vertices[i];
        std::cout << "Vertex " << i << ":" << std::endl;
        std::cout << "  Pos: " << v.position.x << ", " << v.position.y << ", " << v.position.z << std::endl;
        std::cout << "  UV:  " << v.texCoords.x << ", " << v.texCoords.y << std::endl;
    }
}

void MeshComponent::CleanupMesh() {
    if (_vao) glDeleteVertexArrays(1, &_vao);
    if (_vbo) glDeleteBuffers(1, &_vbo);
    if (_ebo) glDeleteBuffers(1, &_ebo);
    _vao = _vbo = _ebo = 0;
}

void MeshComponent::SetMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    CleanupMesh();
    _vertices = vertices;
    _indices = indices;
    SetupMesh();
}

void MeshComponent::OnUpdate() {
    if (_vao == 0 || _vertices.empty() || _indices.empty()) return;

    auto transform = GetOwner()->GetComponent<TransformComponent>();
    if (!transform) return;

    // Store states
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // Apply transform
    const mat4& worldMatrix = transform->GetWorldMatrix();
    glMultMatrixd(glm::value_ptr(worldMatrix));

    // Enable states
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    // Draw using VAO
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Draw normals if enabled
    if (_showNormals) {
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_LIGHTING);
        glColor3f(0.0f, 1.0f, 0.0f);

        glBegin(GL_LINES);
        for (const auto& vertex : _vertices) {
            vec3 start = vertex.position;
            vec3 end = start + (vertex.normal * static_cast<double>(_normalLength));
            glVertex3dv(glm::value_ptr(start));
            glVertex3dv(glm::value_ptr(end));
        }
        glEnd();
    }

    // Restore state
    glPopMatrix();
    glPopAttrib();
}

void MeshComponent::OnInspectorGUI() {
    ImGui::Text("Vertices: %zu", _vertices.size());
    ImGui::Text("Indices: %zu", _indices.size());
    ImGui::Text("Triangles: %zu", _indices.size() / 3);

    if (ImGui::Checkbox("Show Normals", &_showNormals)) {
        // Normal visualization toggled
    }

    if (_showNormals) {
        ImGui::DragFloat("Normal Length", &_normalLength, 0.01f, 0.01f, 1.0f);
    }
}