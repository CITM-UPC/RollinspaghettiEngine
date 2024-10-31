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

    // Create buffers
    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);

    // Convert double precision vertices to float for OpenGL
    std::vector<float> vertexData;
    vertexData.reserve(_vertices.size() * 8); // 3 for pos, 3 for normal, 2 for UV

    for (const auto& vertex : _vertices) {
        // Rotate position by 90 degrees around the X and Y axes simultaneously
        float rotatedX = vertex.position.y;    // New x after rotation
        float rotatedY = -vertex.position.z;   // New y after rotation
        float rotatedZ = -vertex.position.x;   // New z after rotation

        // Position
        vertexData.push_back(rotatedX);
        vertexData.push_back(rotatedY);
        vertexData.push_back(rotatedZ);

        // Normal
        vertexData.push_back(static_cast<float>(vertex.normal.x));
        vertexData.push_back(static_cast<float>(vertex.normal.y));
        vertexData.push_back(static_cast<float>(vertex.normal.z));

        // TexCoords
        vertexData.push_back(static_cast<float>(vertex.texCoords.x));
        vertexData.push_back(static_cast<float>(vertex.texCoords.y));
    }


    // Bind VAO
    glBindVertexArray(_vao);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

    // Set vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    // Unbind
    glBindVertexArray(0);

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

    // Save states
    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glPushMatrix();

    // Apply transform
    const mat4& worldMatrix = transform->GetWorldMatrix();
    glMultMatrixd(glm::value_ptr(worldMatrix));

    // Enable required states
    glEnable(GL_TEXTURE_2D);

    // Use vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Bind vertex data
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexPointer(3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glNormalPointer(GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Bind indices and draw
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);

    // Cleanup states
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glPopMatrix();
    glPopAttrib();

    static bool firstFrame = true;
    if (firstFrame) {
        std::cout << "Drawing mesh with:" << std::endl;
        std::cout << "- Vertices: " << _vertices.size() << std::endl;
        std::cout << "- Indices: " << _indices.size() << std::endl;
        std::cout << "- VBO: " << _vbo << std::endl;
        std::cout << "- EBO: " << _ebo << std::endl;
        std::cout << "- VAO: " << _vao << std::endl;
        firstFrame = false;
    }
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