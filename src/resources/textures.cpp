#include "textures.h"

std::unordered_map<std::string, Texture2D> textures::_data;

Texture2D textures::load_texture(const std::string& filename) {
    auto it = _data.find(filename);
    if (it != _data.end()) {
        return it->second;
    }

    Texture2D texture = LoadTexture(filename.c_str());
    _data.insert({filename, texture});
    return texture;
}
