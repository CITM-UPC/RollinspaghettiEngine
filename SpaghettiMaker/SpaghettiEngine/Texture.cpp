#include "Texture.h"
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <vector>
#include <iostream>
#include <filesystem>

bool Texture::LoadFromFile(const std::string& path) {
    Cleanup();
    _path = path;

    // Initialize DevIL if not done
    static bool initialized = false;
    if (!initialized) {
        ilInit();
        iluInit();
        ilutInit();
        initialized = true;
    }

    // Generate DevIL image ID
    ILuint imageID = 0;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    // Convert to absolute path and then to wide string
    std::filesystem::path fsPath = std::filesystem::absolute(path);
    std::wstring widePath = fsPath.wstring();  // Convert to wide string for DevIL

    std::cout << "Trying to load texture from: " << fsPath.string() << std::endl;

    if (!ilLoadImage(widePath.c_str())) {  // Use wide string version
        // Try relative path as fallback
        std::filesystem::path relativePath(path);
        std::wstring wideRelativePath = relativePath.wstring();
        if (!ilLoadImage(wideRelativePath.c_str())) {
            ILenum error = ilGetError();
            std::cerr << "Failed to load image with DevIL. Error code: " << error << std::endl;
            std::cerr << "Error message: " << error << std::endl;
            ilDeleteImages(1, &imageID);
            return false;
        }
    }

    // Get image info
    _width = ilGetInteger(IL_IMAGE_WIDTH);
    _height = ilGetInteger(IL_IMAGE_HEIGHT);
    _channels = ilGetInteger(IL_IMAGE_CHANNELS);

    std::cout << "Image loaded successfully. Size: " << _width << "x" << _height
        << " Channels: " << _channels << std::endl;

    // Convert to RGBA
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

    // Create OpenGL texture
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    // Set basic parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Upload the texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, ilGetData());

    // Generate mipmaps
    glGenerateMipmap(GL_TEXTURE_2D);

    // Cleanup
    ilDeleteImages(1, &imageID);

    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        std::cerr << "OpenGL error while creating texture: " << error << std::endl;
        return false;
    }

    std::cout << "Texture created successfully. ID: " << _textureID << std::endl;
    _isLoaded = true;
    return true;
}

bool Texture::LoadFromMemory(unsigned char* data, int width, int height, int channels) {
    Cleanup();

    _width = width;
    _height = height;
    _channels = channels;

    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapT);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    GenerateMipmaps();

    _isLoaded = true;
    return true;
}

std::shared_ptr<Texture> Texture::CreateCheckerboard(int width, int height) {
    auto texture = std::make_shared<Texture>();
    std::vector<unsigned char> data(width * height * 4);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            bool isWhite = ((x / 8) + (y / 8)) % 2 == 0;
            unsigned char color = isWhite ? 255 : 128;

            data[(y * width + x) * 4 + 0] = color;  // R
            data[(y * width + x) * 4 + 1] = color;  // G
            data[(y * width + x) * 4 + 2] = color;  // B
            data[(y * width + x) * 4 + 3] = 255;    // A
        }
    }

    texture->LoadFromMemory(data.data(), width, height, 4);
    return texture;
}

void Texture::Bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, _textureID);
}

void Texture::Unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Cleanup() {
    if (_textureID) {
        glDeleteTextures(1, &_textureID);
        _textureID = 0;
    }
    _isLoaded = false;
}

void Texture::SetFiltering(GLint minFilter, GLint magFilter) {
    _minFilter = minFilter;
    _magFilter = magFilter;

    if (_textureID) {
        glBindTexture(GL_TEXTURE_2D, _textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    }
}

void Texture::SetWrapping(GLint wrapS, GLint wrapT) {
    _wrapS = wrapS;
    _wrapT = wrapT;

    if (_textureID) {
        glBindTexture(GL_TEXTURE_2D, _textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
    }
}

void Texture::GenerateMipmaps() {
    if (_textureID) {
        glBindTexture(GL_TEXTURE_2D, _textureID);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
}