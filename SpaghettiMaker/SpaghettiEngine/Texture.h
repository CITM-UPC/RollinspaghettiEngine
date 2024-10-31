#pragma once
#include <string>
#include <GL/glew.h>
#include <memory>
#include <IL/il.h>
#include <IL/ilu.h>

class Texture {
private:
    unsigned int _textureID = 0;
    int _width = 0;
    int _height = 0;
    int _channels = 0;
    std::string _path;
    bool _isLoaded = false;

    // Texture parameters
    GLint _minFilter = GL_LINEAR_MIPMAP_LINEAR;
    GLint _magFilter = GL_LINEAR;
    GLint _wrapS = GL_REPEAT;
    GLint _wrapT = GL_REPEAT;

public:
    Texture() = default;
    ~Texture() { Cleanup(); }

    bool LoadFromFile(const std::string& path);
    bool LoadFromMemory(unsigned char* data, int width, int height, int channels);
    static std::shared_ptr<Texture> CreateCheckerboard(int width = 64, int height = 64);

    void Bind(unsigned int slot = 0) const;
    void Unbind() const;
    void Cleanup();

    // Texture parameter setters
    void SetFiltering(GLint minFilter, GLint magFilter);
    void SetWrapping(GLint wrapS, GLint wrapT);
    void GenerateMipmaps();

    // Getters
    unsigned int GetID() const { return _textureID; }
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
    const std::string& GetPath() const { return _path; }
    bool IsLoaded() const { return _isLoaded; }
};

using TexturePtr = std::shared_ptr<Texture>;