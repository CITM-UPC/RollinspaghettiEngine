#pragma once
#include "GameObject.h"
#include "MeshComponent.h"
#include "types.h"
#include <vector>
#include <memory>

class PrimitiveGenerator {
public:
    // Primitive creation methods
    static GameObject* CreateCube(const char* name = "Cube", float size = 5.0f);
    static GameObject* CreateSphere(const char* name = "Sphere", float radius = 10.0f, int segments = 32);
    //static GameObject* CreateCylinder(const char* name = "Cylinder", float radius = 1.0f, float height = 2.0f, int segments = 32);
    //static GameObject* CreatePlane(const char* name = "Plane", float width = 1.0f, float height = 1.0f);
    //static GameObject* CreateCone(const char* name = "Cone", float radius = 1.0f, float height = 2.0f, int segments = 32);

private:
    // Helper methods for geometry generation
    static void GenerateCubeGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float size);
    static void GenerateSphereGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, int segments);
    //static void GenerateCylinderGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments);
    //static void GeneratePlaneGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float width, float height);
    //static void GenerateConeGeometry(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float radius, float height, int segments);
};