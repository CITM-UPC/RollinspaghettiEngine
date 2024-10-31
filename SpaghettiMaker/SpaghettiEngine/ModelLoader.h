#pragma once
#include "GameObject.h"
#include "Scene.h"
#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class ModelLoader {
public:
    // Add overloaded function that takes texture path
    static GameObject* LoadModel(Scene* scene, const std::string& path, const std::string& texturePath = "");

private:
    static GameObject* ProcessNode(Scene* scene, aiNode* node, const aiScene* scene_ai, GameObject* parent = nullptr, const std::string& texturePath="");
    static void ProcessMesh(GameObject* gameObject, aiMesh* mesh, const aiScene* scene_ai, const std::string& texturePath = "");
    static void ProcessMaterial(GameObject* gameObject, aiMaterial* material, const std::string& texturePath = "");

    // Conversion helpers
    static vec3 AssimpToGlm(const aiVector3D& v);
    static vec2 AssimpToGlm(const aiVector2D& v);
    static vec3 AssimpToGlm(const aiColor3D& c);
};