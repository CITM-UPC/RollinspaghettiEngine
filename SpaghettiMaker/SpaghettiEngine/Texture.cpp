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

    // Convert to absolute path
    std::filesystem::path fsPath = std::filesystem::absolute(std::filesystem::path(path));

    // Check file existence
    if (!std::filesystem::exists(fsPath)) {
        std::cerr << "File does not exist: " << fsPath << std::endl;
        return false;
    }

    auto fileSize = std::filesystem::file_size(fsPath);
    std::cout << "Found file at: " << fsPath << std::endl;
    std::cout << "File size: " << fileSize << " bytes" << std::endl;

    // Generate DevIL image ID
    ILuint imageID = 0;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    // Configure DevIL
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
    ilEnable(IL_ORIGIN_SET);

    // Try different loading approaches
    bool loaded = false;
    std::wstring widePath = fsPath.wstring();

    // Try loading with wide string
    if (ilLoadImage(widePath.c_str())) {
        loaded = true;
        std::cout << "Successfully loaded image with wide path" << std::endl;
    }

    if (!loaded) {
        ILenum error = ilGetError();
        std::cerr << "DevIL error code: " << error << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Get image info
    _width = ilGetInteger(IL_IMAGE_WIDTH);
    _height = ilGetInteger(IL_IMAGE_HEIGHT);
    _channels = ilGetInteger(IL_IMAGE_FORMAT);

    // Print image details
    std::cout << "Image loaded: " << _width << "x" << _height << std::endl;
    std::cout << "Format: " << _channels << std::endl;

    // Convert image format
    if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
        std::cerr << "Failed to convert image format" << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Create OpenGL texture
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, ilGetData());

    glGenerateMipmap(GL_TEXTURE_2D);

    // Check for OpenGL errors
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << glError << std::endl;
    }

    // Cleanup DevIL
    ilDeleteImages(1, &imageID);

    _isLoaded = true;
    std::cout << "Texture loaded successfully. ID: " << _textureID << std::endl;
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