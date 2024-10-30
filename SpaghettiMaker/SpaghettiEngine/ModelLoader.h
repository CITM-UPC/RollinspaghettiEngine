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
    static GameObject* LoadModel(Scene* scene, const std::string& path);

private:
    static GameObject* ProcessNode(Scene* scene, aiNode* node, const aiScene* scene_ai, GameObject* parent = nullptr);
    static void ProcessMesh(GameObject* gameObject, aiMesh* mesh, const aiScene* scene_ai);
    static void ProcessMaterial(GameObject* gameObject, aiMaterial* material);

    // Conversion helpers
    static vec3 AssimpToGlm(const aiVector3D& v);
    static vec2 AssimpToGlm(const aiVector2D& v);
    static vec3 AssimpToGlm(const aiColor3D& c);
};