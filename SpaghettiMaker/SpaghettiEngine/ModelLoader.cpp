#include "ModelLoader.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include <filesystem>
#include <iostream>

GameObject* ModelLoader::LoadModel(Scene* scene, const std::string& path) {
    // Create Assimp importer
    Assimp::Importer importer;

    // Configure importer
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

    // Import scene
    const aiScene* scene_ai = importer.ReadFile(path,
        aiProcess_Triangulate |
        aiProcess_GenNormals |
        aiProcess_CalcTangentSpace |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_FlipUVs  // OpenGL convention
    );

    // Check for errors
    if (!scene_ai || scene_ai->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_ai->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    // Create root game object with the file name
    std::string fileName = std::filesystem::path(path).stem().string();
    GameObject* rootObject = scene->CreateGameObject(fileName.c_str());

    // Process the root node
    ProcessNode(scene, scene_ai->mRootNode, scene_ai, rootObject);

    return rootObject;
}

GameObject* ModelLoader::ProcessNode(Scene* scene, aiNode* node, const aiScene* scene_ai, GameObject* parent) {
    // Create game object for this node
    GameObject* gameObject = scene->CreateGameObject(node->mName.C_Str(), parent);

    // Add transform component and set local transform
    auto transform = gameObject->AddComponent<TransformComponent>();

    // Convert Assimp matrix to glm
    aiMatrix4x4 aiMat = node->mTransformation;
    mat4 glmMat;
    // Transpose because Assimp uses row-major matrices
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            glmMat[i][j] = aiMat[j][i];

    // Decompose transform
    vec3 scale;
    glm::quat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;
    glm::decompose(glmMat, scale, rotation, translation, skew, perspective);

    // Set transform values
    transform->SetLocalPosition(translation);
    transform->SetLocalRotation(rotation);
    transform->SetLocalScale(scale);

    // Process all meshes for this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene_ai->mMeshes[node->mMeshes[i]];
        ProcessMesh(gameObject, mesh, scene_ai);
    }

    // Process children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node->mChildren[i], scene_ai, gameObject);
    }

    return gameObject;
}

void ModelLoader::ProcessMesh(GameObject* gameObject, aiMesh* mesh, const aiScene* scene_ai) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Position
        vertex.position = AssimpToGlm(mesh->mVertices[i]);

        // Normals
        if (mesh->HasNormals()) {
            vertex.normal = AssimpToGlm(mesh->mNormals[i]);
        }

        // Texture coordinates
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
        else {
            vertex.texCoords = vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Create mesh component
    auto meshComp = gameObject->AddComponent<MeshComponent>();
    meshComp->SetMeshData(vertices, indices);

    // Process material
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene_ai->mMaterials[mesh->mMaterialIndex];
        ProcessMaterial(gameObject, material);
    }
}

void ModelLoader::ProcessMaterial(GameObject* gameObject, aiMaterial* material) {
    auto materialComp = gameObject->AddComponent<MaterialComponent>();

    // Load material properties
    aiColor3D color(0.f, 0.f, 0.f);

    if (material->Get(AI_MATKEY_COLOR_AMBIENT, color) == AI_SUCCESS)
        materialComp->SetAmbient(AssimpToGlm(color));

    if (material->Get(AI_MATKEY_COLOR_DIFFUSE, color) == AI_SUCCESS)
        materialComp->SetDiffuse(AssimpToGlm(color));

    if (material->Get(AI_MATKEY_COLOR_SPECULAR, color) == AI_SUCCESS)
        materialComp->SetSpecular(AssimpToGlm(color));

    float shininess;
    if (material->Get(AI_MATKEY_SHININESS, shininess) == AI_SUCCESS)
        materialComp->SetShininess(shininess / 128.0f); // Convert to 0-1 range

    // Load textures
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString path;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
            // Get the directory of the FBX file
            std::filesystem::path texPath = std::filesystem::path(path.C_Str());
            materialComp->SetDiffuseTexture(texPath.string());
        }
    }
}

vec3 ModelLoader::AssimpToGlm(const aiVector3D& v) {
    return vec3(v.x, v.y, v.z);
}

vec2 ModelLoader::AssimpToGlm(const aiVector2D& v) {
    return vec2(v.x, v.y);
}

vec3 ModelLoader::AssimpToGlm(const aiColor3D& c) {
    return vec3(c.r, c.g, c.b);
}