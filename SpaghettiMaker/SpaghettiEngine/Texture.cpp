#include "Texture.h"
#include <GL/glew.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <vector>
#include <iostream>
#include <filesystem>

bool Texture::LoadFromFile(const std::string& path) {
    Cleanup();
    _path = path;

    // Generate DevIL image ID
    ILuint imageID;
    ilGenImages(1, &imageID);
    ilBindImage(imageID);

    // Convert path to wstring for DevIL
    std::filesystem::path fsPath(path);

    // Ensure the file exists
    if (!std::filesystem::exists(fsPath)) {
        std::cerr << "Texture file does not exist: " << path << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Print absolute path for debugging
    std::cout << "Attempting to load texture in Texture.cpp from: " << std::filesystem::absolute(fsPath).string() << std::endl;

    // Create wide string path
    std::wstring widePath = fsPath.wstring();

    // Load image - Debug the return value
    ILboolean success = ilLoadImage(widePath.c_str());
    if (!success) {
        ILenum error = ilGetError();
        // Convert error code to string without using iluErrorString
        std::cerr << "DevIL error code: " << error << std::endl;
        std::cerr << "Failed to load texture: " << path << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Get original format information
    ILint format = ilGetInteger(IL_IMAGE_FORMAT);
    ILint type = ilGetInteger(IL_IMAGE_TYPE);
    std::cout << "Original image format: " << format << ", type: " << type << std::endl;




    // Convert image to RGBA format
    if (!ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE)) {
        ILenum error = ilGetError();
        std::cerr << "Failed to convert image. Error code: " << error << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Get image dimensions
    _width = ilGetInteger(IL_IMAGE_WIDTH);
    _height = ilGetInteger(IL_IMAGE_HEIGHT);
    _channels = 4; // RGBA

    std::cout << "Successfully loaded image in Texure.cpp: " << path << std::endl;
    std::cout << "Dimensions: " << _width << "x" << _height << std::endl;

    // Create OpenGL texture
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _magFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, _wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, _wrapT);

    // Get the image data
    ILubyte* data = ilGetData();
    if (!data) {
        std::cerr << "Failed to get image data" << std::endl;
        ilDeleteImages(1, &imageID);
        return false;
    }

    // Upload texture data to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    // Check for OpenGL errors
    GLenum glError = glGetError();
    if (glError != GL_NO_ERROR) {
        std::cerr << "OpenGL error when uploading texture: " << glError << std::endl;
    }
    else {
        std::cout << "Texture uploaded to OpenGL successfully" << std::endl;
    }

    GenerateMipmaps();

    // Cleanup DevIL image
    ilDeleteImages(1, &imageID);

    // Verify texture creation
    if (_textureID == 0) {
        std::cerr << "Failed to create OpenGL texture" << std::endl;
        return false;
    }

    _isLoaded = true;
    std::cout << "Texture loading completed successfully in Texure.cpp. ID: " << _textureID << std::endl;

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