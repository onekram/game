#include "sounds.h"

std::unordered_map<std::string, Sound> sounds::_data;

Sound sounds::load_sound(const std::string& filename) {
    auto it = _data.find(filename);
    if (it != _data.end()) {
        return it->second;
    }

    Sound texture = LoadSound(filename.c_str());
    _data.insert({filename, texture});
    return texture;
}
