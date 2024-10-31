#include "TextureManager.h"
#include "imgui.h"
#include <filesystem>
#include <iostream>

TextureManager* TextureManager::s_instance = nullptr;

void TextureManager::Initialize() {
    // Create default checker texture
    _defaultTexture = Texture::CreateCheckerboard(64, 64);

    // Add it to cache
    _textureCache["__default"] = _defaultTexture;
}

void TextureManager::Cleanup() {
    _textureCache.clear();
    _defaultTexture = nullptr;
}

TexturePtr TextureManager::LoadTexture(const std::string& path) {
    // Normalize path
    std::filesystem::path fsPath(path);

    try {
        fsPath = std::filesystem::absolute(fsPath);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Path error: " << e.what() << std::endl;
        return _defaultTexture;
    }

    std::string normalizedPath = fsPath.string();
    std::cout << "Attempting to load texture from normalized path: " << normalizedPath << std::endl;

    // Check if texture is already loaded
    auto it = _textureCache.find(normalizedPath);
    if (it != _textureCache.end()) {
        std::cout << "Found cached texture: " << normalizedPath << std::endl;
        return it->second;
    }

    // Load new texture
    auto texture = std::make_shared<Texture>();
    if (texture->LoadFromFile(normalizedPath)) {
        _textureCache[normalizedPath] = texture;
        std::cout << "Successfully loaded and cached new texture: " << normalizedPath << std::endl;
        return texture;
    }

    std::cerr << "Failed to load texture, falling back to default: " << normalizedPath << std::endl;
    return _defaultTexture;
}

TexturePtr TextureManager::GetTexture(const std::string& path) const {
    std::filesystem::path fsPath(path);
    std::string normalizedPath = fsPath.lexically_normal().string();

    auto it = _textureCache.find(normalizedPath);
    return (it != _textureCache.end()) ? it->second : _defaultTexture;
}

void TextureManager::UnloadTexture(const std::string& path) {
    std::filesystem::path fsPath(path);
    std::string normalizedPath = fsPath.lexically_normal().string();

    auto it = _textureCache.find(normalizedPath);
    if (it != _textureCache.end()) {
        _textureCache.erase(it);
    }
}

void TextureManager::UnloadUnusedTextures() {
    for (auto it = _textureCache.begin(); it != _textureCache.end();) {
        // Don't unload default texture
        if (it->first == "__default") {
            ++it;
            continue;
        }

        // Check if texture is only referenced by the cache
        if (it->second.use_count() == 1) {
            it = _textureCache.erase(it);
        }
        else {
            ++it;
        }
    }
}

void TextureManager::OnImGuiRender() {
    if (ImGui::Begin("Texture Manager")) {
        ImGui::Text("Loaded Textures: %zu", _textureCache.size());

        if (ImGui::Button("Unload Unused")) {
            UnloadUnusedTextures();
        }

        ImGui::Separator();

        for (const auto& [path, texture] : _textureCache) {
            ImGui::PushID(path.c_str());

            bool isDefault = (path == "__default");
            std::string displayName = isDefault ? "Default (Checker)" : std::filesystem::path(path).filename().string();

            if (ImGui::TreeNode(displayName.c_str())) {
                ImGui::Text("Path: %s", path.c_str());
                ImGui::Text("Size: %dx%d", texture->GetWidth(), texture->GetHeight());

                // Fixed: Get use_count from the shared_ptr in the cache
                auto refCount = _textureCache[path].use_count();
                ImGui::Text("References: %d", static_cast<int>(refCount));

                // Preview texture
                ImTextureID texId = reinterpret_cast<ImTextureID>(static_cast<uintptr_t>(texture->GetID()));
                ImGui::Image(texId, ImVec2(100, 100));

                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    }
    ImGui::End();
}