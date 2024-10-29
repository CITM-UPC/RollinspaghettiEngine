#include "PrimitiveGenerator.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "GameObject.h"
#include <glm/gtc/constants.hpp>

GameObject* PrimitiveGenerator::CreateCube(const char* name, float size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateCubeGeometry(vertices, indices, size);

    GameObject* cube = new GameObject(name);

    // Add required components
    auto transform = cube->AddComponent<TransformComponent>();
    auto mesh = cube->AddComponent<MeshComponent>();
    auto material = cube->AddComponent<MaterialComponent>();

    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    return cube;
}

GameObject* PrimitiveGenerator::CreateSphere(const char* name, float radius, int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateSphereGeometry(vertices, indices, radius, segments);

    GameObject* sphere = new GameObject(name);

    auto transform = sphere->AddComponent<TransformComponent>();
    auto mesh = sphere->AddComponent<MeshComponent>();
    auto material = sphere->AddComponent<MaterialComponent>();

    mesh->SetMeshData(vertices, indices);

    return sphere;
}

void PrimitiveGenerator::GenerateCubeGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float size) {
    float halfSize = size * 0.5f;

    // Vertices for all 6 faces
    vertices = {
        // Front face
        Vertex(vec3(-halfSize, -halfSize, halfSize), vec3(0, 0, 1), vec2(0, 0)),
        Vertex(vec3(halfSize, -halfSize, halfSize), vec3(0, 0, 1), vec2(1, 0)),
        Vertex(vec3(halfSize, halfSize, halfSize), vec3(0, 0, 1), vec2(1, 1)),
        Vertex(vec3(-halfSize, halfSize, halfSize), vec3(0, 0, 1), vec2(0, 1)),

        // Back face
        Vertex(vec3(-halfSize, -halfSize, -halfSize), vec3(0, 0, -1), vec2(1, 0)),
        Vertex(vec3(-halfSize, halfSize, -halfSize), vec3(0, 0, -1), vec2(1, 1)),
        Vertex(vec3(halfSize, halfSize, -halfSize), vec3(0, 0, -1), vec2(0, 1)),
        Vertex(vec3(halfSize, -halfSize, -halfSize), vec3(0, 0, -1), vec2(0, 0)),

        // Top face
        Vertex(vec3(-halfSize, halfSize, -halfSize), vec3(0, 1, 0), vec2(0, 1)),
        Vertex(vec3(-halfSize, halfSize, halfSize), vec3(0, 1, 0), vec2(0, 0)),
        Vertex(vec3(halfSize, halfSize, halfSize), vec3(0, 1, 0), vec2(1, 0)),
        Vertex(vec3(halfSize, halfSize, -halfSize), vec3(0, 1, 0), vec2(1, 1)),

        // Bottom face
        Vertex(vec3(-halfSize, -halfSize, -halfSize), vec3(0, -1, 0), vec2(0, 0)),
        Vertex(vec3(halfSize, -halfSize, -halfSize), vec3(0, -1, 0), vec2(1, 0)),
        Vertex(vec3(halfSize, -halfSize, halfSize), vec3(0, -1, 0), vec2(1, 1)),
        Vertex(vec3(-halfSize, -halfSize, halfSize), vec3(0, -1, 0), vec2(0, 1)),

        // Right face
        Vertex(vec3(halfSize, -halfSize, -halfSize), vec3(1, 0, 0), vec2(1, 0)),
        Vertex(vec3(halfSize, halfSize, -halfSize), vec3(1, 0, 0), vec2(1, 1)),
        Vertex(vec3(halfSize, halfSize, halfSize), vec3(1, 0, 0), vec2(0, 1)),
        Vertex(vec3(halfSize, -halfSize, halfSize), vec3(1, 0, 0), vec2(0, 0)),

        // Left face
        Vertex(vec3(-halfSize, -halfSize, -halfSize), vec3(-1, 0, 0), vec2(0, 0)),
        Vertex(vec3(-halfSize, -halfSize, halfSize), vec3(-1, 0, 0), vec2(1, 0)),
        Vertex(vec3(-halfSize, halfSize, halfSize), vec3(-1, 0, 0), vec2(1, 1)),
        Vertex(vec3(-halfSize, halfSize, -halfSize), vec3(-1, 0, 0), vec2(0, 1))
    };

    // Indices for all 6 faces
    indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Top face
        8, 9, 10, 10, 11, 8,
        // Bottom face
        12, 13, 14, 14, 15, 12,
        // Right face
        16, 17, 18, 18, 19, 16,
        // Left face
        20, 21, 22, 22, 23, 20
    };
}

void PrimitiveGenerator::GenerateSphereGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, int segments) {
    vertices.clear();
    indices.clear();

    // Generate vertices
    for (int lat = 0; lat <= segments; lat++) {
        float theta = lat * glm::pi<float>() / segments;
        float sinTheta = std::sin(theta);
        float cosTheta = std::cos(theta);

        for (int lon = 0; lon <= segments; lon++) {
            float phi = lon * 2.0f * glm::pi<float>() / segments;
            float sinPhi = std::sin(phi);
            float cosPhi = std::cos(phi);

            vec3 position(
                radius * sinTheta * cosPhi,
                radius * cosTheta,
                radius * sinTheta * sinPhi
            );

            vec3 normal = glm::normalize(position);
            vec2 uv(static_cast<float>(lon) / segments, static_cast<float>(lat) / segments);

            vertices.emplace_back(position, normal, uv);
        }
    }

    // Generate indices
    for (int lat = 0; lat < segments; lat++) {
        for (int lon = 0; lon < segments; lon++) {
            unsigned int current = lat * (segments + 1) + lon;
            unsigned int next = current + segments + 1;

            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            indices.push_back(next);
            indices.push_back(next + 1);
            indices.push_back(current + 1);
        }
    }
}

// Similar implementations for cylinder, plane, and cone...