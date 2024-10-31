#include "ModelLoader.h"
#include "MeshComponent.h"
#include "MaterialComponent.h"
#include "TransformComponent.h"
#include <filesystem>
#include <iostream>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include "ConsoleWindow.h"

GameObject* ModelLoader::LoadModel(Scene* scene, const std::string& path, const std::string& texturePath)
{
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
        aiProcess_FlipUVs | // OpenGL convention
        aiProcess_GlobalScale |  // Add global scaling
        aiProcess_PreTransformVertices  // Pre-transform vertices to fix scaling issues
    );

    // Check for errors
    if (!scene_ai || scene_ai->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene_ai->mRootNode) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    // Create root game object with the file name
    std::string fileName = std::filesystem::path(path).stem().string();
    GameObject* rootObject = scene->CreateGameObject(fileName.c_str());

    // Add initial transform to root
    auto rootTransform = rootObject->AddComponent<TransformComponent>();

    // Set a default scale that works well with most models
    rootTransform->SetLocalScale(vec3(0.1));  // Scale down by default

    // Handle texture path
    std::string finalTexturePath;
    if (!texturePath.empty()) {
        // If explicit texture path provided, make it absolute
        std::filesystem::path texPath(texturePath);
        if (texPath.is_relative()) {
            // If relative, make it relative to model path
            std::filesystem::path modelDir = std::filesystem::absolute(std::filesystem::path(path)).parent_path();
            texPath = modelDir / texPath;
        }
        finalTexturePath = texPath.string();
    }
    else {
        // Try to find texture in same directory as model
        std::filesystem::path modelDir = std::filesystem::absolute(std::filesystem::path(path)).parent_path();
        std::filesystem::path defaultTexPath = modelDir / "Baker_house.png";
        if (std::filesystem::exists(defaultTexPath)) {
            finalTexturePath = defaultTexPath.string();
        }
    }

    std::cout << "Model path: " << std::filesystem::absolute(path) << std::endl;
    std::cout << "Texture path: " << finalTexturePath << std::endl;

    // Process the root node
    ProcessNode(scene, scene_ai->mRootNode, scene_ai, rootObject, finalTexturePath);

    std::cout << "Loading model: " << path << std::endl;
    std::cout << "Number of meshes: " << scene_ai->mNumMeshes << std::endl;
    std::cout << "Number of materials: " << scene_ai->mNumMaterials << std::endl;

    return rootObject;
}

GameObject* ModelLoader::ProcessNode(Scene* scene, aiNode* node, const aiScene* scene_ai, GameObject* parent, const std::string& texturePath) {
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
    glm::dquat rotation;
    vec3 translation;
    vec3 skew;
    vec4 perspective;

    // Use explicit types for decompose
    bool success = glm::decompose(
        glmMat,
        scale,
        rotation,
        translation,
        skew,
        perspective
    );

    // Set transform values
    if (success) {
        transform->SetLocalPosition(translation);
        transform->SetLocalRotation(rotation);
        transform->SetLocalScale(scale);
    }
    else {
        std::cerr << "Failed to decompose transformation matrix for node: " << node->mName.C_Str() << std::endl;
        transform->SetLocalPosition(vec3(0.0));
        transform->SetLocalRotation(glm::dquat(1.0, 0.0, 0.0, 0.0));
        transform->SetLocalScale(vec3(1.0));
    }

    // Process all meshes for this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene_ai->mMeshes[node->mMeshes[i]];
        ProcessMesh(gameObject, mesh, scene_ai, texturePath);
    }

    // Process children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(scene, node->mChildren[i], scene_ai, gameObject, texturePath);
    }

    // Debug output for transforms
    std::cout << "Processing node: " << node->mName.C_Str() << std::endl;
    std::cout << "Transform matrix:" << std::endl;
    for (int i = 0; i < 4; i++) {
        std::cout << glmMat[i][0] << ", " << glmMat[i][1] << ", "
            << glmMat[i][2] << ", " << glmMat[i][3] << std::endl;
    }
    std::cout << "Position: " << translation.x << ", " << translation.y << ", " << translation.z << std::endl;
    std::cout << "Scale: " << scale.x << ", " << scale.y << ", " << scale.z << std::endl;

    return gameObject;
}

void ModelLoader::ProcessMesh(GameObject* gameObject, aiMesh* mesh, const aiScene* scene_ai, const std::string& texturePath) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Print mesh info
    std::cout << "Processing Mesh: " << mesh->mName.C_Str() << std::endl;
    std::cout << "Number of Vertices: " << mesh->mNumVertices << std::endl;
    std::cout << "Number of Faces: " << mesh->mNumFaces << std::endl;

    // Process vertices
    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;

        // Convert to GLM vectors
        vertex.position = AssimpToGlm(mesh->mVertices[i]);

        if (mesh->HasNormals()) {
            vertex.normal = glm::normalize(AssimpToGlm(mesh->mNormals[i]));
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }

        vertices.push_back(vertex);
    }

    // Process indices
    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Create and setup components
    auto meshComp = gameObject->AddComponent<MeshComponent>();
    meshComp->SetMeshData(vertices, indices);

    // Process material
    if (mesh->mMaterialIndex >= 0) {
        std::string customTexturePath = texturePath; // Specify your custom texture path
        ProcessMaterial(gameObject, scene_ai->mMaterials[mesh->mMaterialIndex], customTexturePath);
    }

    std::cout << "Processing mesh with " << mesh->mNumVertices << " vertices and "
        << mesh->mNumFaces << " faces" << std::endl;

}

void ModelLoader::ProcessMaterial(GameObject* gameObject, aiMaterial* material, const std::string& texturePath) {
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

    // Add debug logging
    std::cout << "Processing material for: " << gameObject->GetName() << std::endl;
    std::cout << "Custom texture path: " << texturePath << std::endl;

    // Try loading texture
    if (!texturePath.empty()) {
        if (materialComp->SetDiffuseTexture(texturePath)) {
            std::cout << "Successfully applied custom texture: " << texturePath << std::endl;
        }
        else {
            std::cerr << "Failed to apply custom texture: " << texturePath << std::endl;
        }
    }
    else {
        // Try to load embedded texture
        if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString path;
            if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS) {
                std::string texPath = path.C_Str();
                std::cout << "Found embedded texture path: " << texPath << std::endl;
                materialComp->SetDiffuseTexture(texPath);
            }
        }
        else {
            std::cout << "No texture found for material, using default" << std::endl;
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