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

    // Bind VAO first
    glBindVertexArray(_vao);

    // Bind and set vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), _vertices.data(), GL_STATIC_DRAW);

    // Bind and set element buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);

    // Set vertex attribute pointers
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // TexCoords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

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

    // Store current OpenGL state
    GLboolean prevLighting = glIsEnabled(GL_LIGHTING);
    GLboolean prevTexture2D = glIsEnabled(GL_TEXTURE_2D);

    // Draw mesh using vertex buffer objects
    glBindVertexArray(_vao);

    // Enable vertex arrays
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Set up vertex pointers
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glVertexPointer(3, GL_DOUBLE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glNormalPointer(GL_DOUBLE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glTexCoordPointer(2, GL_DOUBLE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    // Draw elements
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);

    // Cleanup state
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    //// Apply transform
    //glPushMatrix();
    //glMultMatrixd(glm::value_ptr(transform->GetWorldMatrix()));

    //// Draw mesh
    //glBindVertexArray(_vao);
    //glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(_indices.size()), GL_UNSIGNED_INT, 0);

    // Draw normals if enabled
    if (_showNormals) {
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);

        glBegin(GL_LINES);
        glColor3f(0.0f, 1.0f, 0.0f); // Green for normals

        for (const auto& vertex : _vertices) {
            vec3 start = vertex.position;
            // Fixed: Multiply vec3 by scalar using proper GLM syntax
            vec3 end = start + (vertex.normal * static_cast<double>(_normalLength));

            glVertex3dv(glm::value_ptr(start));
            glVertex3dv(glm::value_ptr(end));
        }
        glEnd();

        // Restore state
        if (prevLighting) glEnable(GL_LIGHTING);
        if (prevTexture2D) glEnable(GL_TEXTURE_2D);
    }

    glBindVertexArray(0);
    //glPopMatrix();

    std::cout << "Rendering mesh with " << _vertices.size() << " vertices and "
        << _indices.size() << " indices" << std::endl;
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