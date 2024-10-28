#pragma once
#include "Texture.h"
#include <unordered_map>
#include <memory>
#include <string>

class TextureManager {
private:
    static TextureManager* s_instance;
    std::unordered_map<std::string, TexturePtr> _textureCache;
    TexturePtr _defaultTexture; // Checker texture

    // Private constructor for singleton
    TextureManager() = default;

public:
    static TextureManager* GetInstance() {
        if (!s_instance) {
            s_instance = new TextureManager();
        }
        return s_instance;
    }

    static void Destroy() {
        delete s_instance;
        s_instance = nullptr;
    }

    // Initialize manager and create default textures
    void Initialize();
    void Cleanup();

    // Resource management
    TexturePtr LoadTexture(const std::string& path);
    TexturePtr GetTexture(const std::string& path) const;
    void UnloadTexture(const std::string& path);
    void UnloadUnusedTextures(); // Removes textures with only one reference (the cache)

    // Default texture access
    TexturePtr GetDefaultTexture() const { return _defaultTexture; }

    // Debug/Editor functions
    void OnImGuiRender(); // Display loaded textures in editor
    size_t GetLoadedTextureCount() const { return _textureCache.size(); }
};

#define TEXTURE_MANAGER TextureManager::GetInstance()