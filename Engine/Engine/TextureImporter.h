// TextureImporter.h
#pragma once

#include "Resource.h"
#include "Texture.h"
#include <GL/glew.h>
#include <unordered_map>
#include <string>

class TextureImporter
{
public:
    TextureImporter();
    ~TextureImporter();

    void SaveTextureFile(Resource* resource);
    Texture* LoadTextureImage(Resource* resource);
    GLuint LoadIconImage(const std::string& filePath);

    // Nuevos métodos para el manejo de caché
    static void ClearTextureCache();

private:
    static std::unordered_map<std::string, Texture*> textureCache;
};