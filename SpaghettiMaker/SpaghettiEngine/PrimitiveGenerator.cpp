#include "PrimitiveGenerator.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include "Renderer.h"
#include "GameObject.h"
#include "RendererComponent.h"
#include <glm/gtc/constants.hpp>
#include <cmath> // Add this include for M_PI

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

GameObject* PrimitiveGenerator::CreateCube(const char* name, float size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateCubeGeometry(vertices, indices, size);

    GameObject* cube = new GameObject(name);

    // Add required components
    auto transform = cube->AddComponent<TransformComponent>();
    auto mesh = cube->AddComponent<MeshComponent>();
    auto material = cube->AddComponent<MaterialComponent>();
    auto renderer = cube->AddComponent<RendererComponent>();  // Add renderer


    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    // Initialize components
    transform->OnStart();
    mesh->OnStart();
    material->OnStart();
    renderer->OnStart();

    return cube;
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

GameObject* PrimitiveGenerator::CreateCylinder(const char* name, float radius, float height, int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateCylinderGeometry(vertices, indices, radius, height, segments);

    GameObject* cylinder = new GameObject(name);

    // Add required components
    auto transform = cylinder->AddComponent<TransformComponent>();
    auto mesh = cylinder->AddComponent<MeshComponent>();
    auto material = cylinder->AddComponent<MaterialComponent>();
    auto renderer = cylinder->AddComponent<RendererComponent>();  // Add renderer

    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    // Initialize components
    transform->OnStart();
    mesh->OnStart();
    material->OnStart();
    renderer->OnStart();

    return cylinder;
}

void PrimitiveGenerator::GenerateCylinderGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments) {
    float halfHeight = height * 0.5f;
    float angleStep = 2.0f * M_PI / segments;

    // Generate vertices and indices for the top and bottom circular faces
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // Top face vertex
        vertices.emplace_back(vec3(x, halfHeight, z), vec3(0, 1, 0), vec2((float)i / segments, 1.0f));
        // Bottom face vertex
        vertices.emplace_back(vec3(x, -halfHeight, z), vec3(0, -1, 0), vec2((float)i / segments, 0.0f));
    }

    // Center vertices for the top and bottom faces
    vertices.emplace_back(vec3(0, halfHeight, 0), vec3(0, 1, 0), vec2(0.5f, 0.5f));     // Top center
    vertices.emplace_back(vec3(0, -halfHeight, 0), vec3(0, -1, 0), vec2(0.5f, 0.5f));  // Bottom center

    // Indices for the top and bottom circular faces
    int topCenterIndex = segments * 2;     // Index of top center vertex
    int bottomCenterIndex = topCenterIndex + 1;  // Index of bottom center vertex

    for (int i = 0; i < segments; ++i) {
        int topIndex = i * 2;
        int bottomIndex = i * 2 + 1;
        int nextTopIndex = (i + 1) * 2 % (segments * 2);
        int nextBottomIndex = (i + 1) * 2 + 1 % (segments * 2);

        // Top face triangle
        indices.push_back(topIndex);
        indices.push_back(nextTopIndex);
        indices.push_back(topCenterIndex);

        // Bottom face triangle
        indices.push_back(bottomCenterIndex);
        indices.push_back(nextBottomIndex);
        indices.push_back(bottomIndex);
    }

    // Generate vertices and indices for the side faces
    for (int i = 0; i < segments; ++i) {
        int topIndex = i * 2;
        int bottomIndex = i * 2 + 1;
        int nextTopIndex = (i + 1) * 2 % (segments * 2);
        int nextBottomIndex = (i + 1) * 2 + 1 % (segments * 2);

        // First triangle
        indices.push_back(topIndex);
        indices.push_back(nextBottomIndex);
        indices.push_back(bottomIndex);

        // Second triangle
        indices.push_back(topIndex);
        indices.push_back(nextTopIndex);
        indices.push_back(nextBottomIndex);
    }
}

GameObject* PrimitiveGenerator::CreatePlane(const char* name, float width, float height) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GeneratePlaneGeometry(vertices, indices, width, height);

    GameObject* plane = new GameObject(name);

    // Add required components
    auto transform = plane->AddComponent<TransformComponent>();
    auto mesh = plane->AddComponent<MeshComponent>();
    auto material = plane->AddComponent<MaterialComponent>();
    auto renderer = plane->AddComponent<RendererComponent>();  // Add renderer

    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    // Initialize components
    transform->OnStart();
    mesh->OnStart();
    material->OnStart();
    renderer->OnStart();

    return plane;
}

void PrimitiveGenerator::GeneratePlaneGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float width, float height) {
    float halfWidth = width * 0.5f;
    float halfHeight = height * 0.5f;

    // Define vertices for the plane (centered on origin)
    vertices = {
        Vertex(vec3(-halfWidth, 0, halfHeight), vec3(0, 1, 0), vec2(0, 1)),  // Top-left
        Vertex(vec3(halfWidth, 0, halfHeight), vec3(0, 1, 0), vec2(1, 1)),   // Top-right
        Vertex(vec3(halfWidth, 0, -halfHeight), vec3(0, 1, 0), vec2(1, 0)),  // Bottom-right
        Vertex(vec3(-halfWidth, 0, -halfHeight), vec3(0, 1, 0), vec2(0, 0))  // Bottom-left
    };

    // Define indices for two triangles that form the plane quad
    indices = {
        0, 1, 2,  // First triangle
        2, 3, 0   // Second triangle
    };
}

GameObject* PrimitiveGenerator::CreateCone(const char* name, float radius, float height, int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateConeGeometry(vertices, indices, radius, height, segments);

    GameObject* cone = new GameObject(name);

    // Add required components
    auto transform = cone->AddComponent<TransformComponent>();
    auto mesh = cone->AddComponent<MeshComponent>();
    auto material = cone->AddComponent<MaterialComponent>();
    auto renderer = cone->AddComponent<RendererComponent>();  // Add renderer

    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    // Initialize components
    transform->OnStart();
    mesh->OnStart();
    material->OnStart();
    renderer->OnStart();

    return cone;
}

void PrimitiveGenerator::GenerateConeGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments) {
    float halfHeight = height * 0.5f;
    float angleStep = 2.0f * M_PI / segments;

    // Apex vertex
    vertices.emplace_back(vec3(0, halfHeight, 0), vec3(0, 1, 0), vec2(0.5f, 1.0f));
    int apexIndex = 0;

    // Base center vertex
    vertices.emplace_back(vec3(0, -halfHeight, 0), vec3(0, -1, 0), vec2(0.5f, 0.5f));
    int baseCenterIndex = 1;

    // Generate vertices for the base circle
    for (int i = 0; i <= segments; ++i) {
        float angle = i * angleStep;
        float x = cos(angle) * radius;
        float z = sin(angle) * radius;

        // Vertex on the base circle
        vertices.emplace_back(vec3(x, -halfHeight, z), vec3(0, -1, 0), vec2((cos(angle) + 1) * 0.5f, (sin(angle) + 1) * 0.5f));
    }

    // Indices for the base of the cone (using a triangle fan)
    for (int i = 2; i < segments + 2; ++i) {
        indices.push_back(baseCenterIndex);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    // Indices for the sides of the cone
    for (int i = 2; i < segments + 2; ++i) {
        indices.push_back(apexIndex);
        indices.push_back(i + 1);
        indices.push_back(i);
    }
}


GameObject* PrimitiveGenerator::CreateCapsule(const char* name, float radius, float height, int segments, int rings) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateCapsuleGeometry(vertices, indices, radius, height, segments, rings);

    GameObject* capsule = new GameObject(name);

    // Add required components
    auto transform = capsule->AddComponent<TransformComponent>();
    auto mesh = capsule->AddComponent<MeshComponent>();
    auto material = capsule->AddComponent<MaterialComponent>();
    auto renderer = capsule->AddComponent<RendererComponent>();  // Add renderer

    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    // Initialize components
    transform->OnStart();
    mesh->OnStart();
    material->OnStart();
    renderer->OnStart();

    return capsule;
}

void PrimitiveGenerator::GenerateCapsuleGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments, int rings) {
    float halfHeight = height * 0.5f - radius;  // Effective cylinder height between hemispheres
    float angleStepH = 2.0f * M_PI / segments;  // Horizontal angle step
    float angleStepV = M_PI / (rings * 2);      // Vertical angle step (for hemispheres)

    // Generate vertices and indices for the top hemisphere
    for (int ring = 0; ring <= rings; ++ring) {
        float verticalAngle = ring * angleStepV;
        float ringRadius = radius * sin(verticalAngle);
        float y = halfHeight + radius * cos(verticalAngle);  // Adjusted height

        for (int segment = 0; segment <= segments; ++segment) {
            float horizontalAngle = segment * angleStepH;
            float x = ringRadius * cos(horizontalAngle);
            float z = ringRadius * sin(horizontalAngle);

            // Top hemisphere vertex
            vertices.emplace_back(vec3(x, y, z), glm::normalize(vec3(x, cos(verticalAngle), z)), vec2((float)segment / segments, (float)ring / rings));
        }
    }

    // Generate vertices and indices for the cylindrical body
    for (int segment = 0; segment <= segments; ++segment) {
        float horizontalAngle = segment * angleStepH;
        float x = radius * cos(horizontalAngle);
        float z = radius * sin(horizontalAngle);

        // Top edge of the cylinder
        vertices.emplace_back(vec3(x, halfHeight, z), glm::normalize(vec3(x, 0, z)), vec2((float)segment / segments, 0.5f));
        // Bottom edge of the cylinder
        vertices.emplace_back(vec3(x, -halfHeight, z), glm::normalize(vec3(x, 0, z)), vec2((float)segment / segments, 0.5f));
    }

    // Generate vertices and indices for the bottom hemisphere
    for (int ring = 0; ring <= rings; ++ring) {
        float verticalAngle = ring * angleStepV;
        float ringRadius = radius * sin(verticalAngle);
        float y = -halfHeight - radius * cos(verticalAngle);  // Adjusted height

        for (int segment = 0; segment <= segments; ++segment) {
            float horizontalAngle = segment * angleStepH;
            float x = ringRadius * cos(horizontalAngle);
            float z = ringRadius * sin(horizontalAngle);

            // Bottom hemisphere vertex
            vertices.emplace_back(vec3(x, y, z), glm::normalize(vec3(x, -cos(verticalAngle), z)), vec2((float)segment / segments, (float)(ring + rings) / (2 * rings)));
        }
    }

    // Generate indices for the top hemisphere
    for (int ring = 0; ring < rings; ++ring) {
        for (int segment = 0; segment < segments; ++segment) {
            int topLeft = ring * (segments + 1) + segment;
            int topRight = topLeft + 1;
            int bottomLeft = (ring + 1) * (segments + 1) + segment;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
        }
    }

    // Generate indices for the cylinder body
    int cylinderOffset = (rings + 1) * (segments + 1);
    for (int segment = 0; segment < segments; ++segment) {
        int top = cylinderOffset + segment * 2;
        int bottom = top + 1;
        int nextTop = top + 2;
        int nextBottom = bottom + 2;

        indices.push_back(top);
        indices.push_back(bottom);
        indices.push_back(nextBottom);

        indices.push_back(top);
        indices.push_back(nextBottom);
        indices.push_back(nextTop);
    }

    // Generate indices for the bottom hemisphere
    int bottomHemisphereOffset = cylinderOffset + (segments + 1) * 2;
    for (int ring = 0; ring < rings; ++ring) {
        for (int segment = 0; segment < segments; ++segment) {
            int topLeft = bottomHemisphereOffset + ring * (segments + 1) + segment;
            int topRight = topLeft + 1;
            int bottomLeft = bottomHemisphereOffset + (ring + 1) * (segments + 1) + segment;
            int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);

            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(topRight);
        }
    }
}

GameObject* PrimitiveGenerator::CreateTorus(const char* name, float ringRadius, float tubeRadius, int ringSegments, int tubeSegments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    GenerateTorusGeometry(vertices, indices, ringRadius, tubeRadius, ringSegments, tubeSegments);

    GameObject* torus = new GameObject(name);

    // Add required components
    auto transform = torus->AddComponent<TransformComponent>();
    auto mesh = torus->AddComponent<MeshComponent>();
    auto material = torus->AddComponent<MaterialComponent>();
    auto renderer = torus->AddComponent<RendererComponent>();  // Add renderer

    // Set mesh data
    mesh->SetMeshData(vertices, indices);

    // Initialize components
    transform->OnStart();
    mesh->OnStart();
    material->OnStart();
    renderer->OnStart();

    return torus;
}

void PrimitiveGenerator::GenerateTorusGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float ringRadius, float tubeRadius, int ringSegments, int tubeSegments) {
    float ringStep = 2.0f * M_PI / ringSegments;
    float tubeStep = 2.0f * M_PI / tubeSegments;

    // Generate vertices
    for (int ring = 0; ring <= ringSegments; ++ring) {
        float ringAngle = ring * ringStep;
        float cosRing = cos(ringAngle);
        float sinRing = sin(ringAngle);

        // Center position of the current ring
        vec3 ringCenter(cosRing * ringRadius, 0, sinRing * ringRadius);

        for (int tube = 0; tube <= tubeSegments; ++tube) {
            float tubeAngle = tube * tubeStep;
            float cosTube = cos(tubeAngle);
            float sinTube = sin(tubeAngle);

            // Calculate vertex position
            vec3 position = ringCenter + vec3(cosRing * tubeRadius * cosTube, tubeRadius * sinTube, sinRing * tubeRadius * cosTube);

            // Calculate normal for lighting
            vec3 normal = glm :: normalize(vec3(cosRing * cosTube, sinTube, sinRing * cosTube));

            // UV coordinates for texturing
            vec2 uv((float)ring / ringSegments, (float)tube / tubeSegments);

            // Add vertex
            vertices.emplace_back(position, normal, uv);
        }
    }

    // Generate indices
    for (int ring = 0; ring < ringSegments; ++ring) {
        for (int tube = 0; tube < tubeSegments; ++tube) {
            int current = ring * (tubeSegments + 1) + tube;
            int next = (ring + 1) * (tubeSegments + 1) + tube;

            // First triangle of quad
            indices.push_back(current);
            indices.push_back(next);
            indices.push_back(current + 1);

            // Second triangle of quad
            indices.push_back(current + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }
}

// Similar implementations for cylinder, plane, and cone...