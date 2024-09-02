#include "sounds.h"

std::unordered_map<std::string, Sound> sounds::_data_sound;
std::unordered_map<std::string, Music> sounds::_data_music;

Sound sounds::load_sound(const std::string& filename) {
    auto it = _data_sound.find(filename);
    if (it != _data_sound.end()) {
        return it->second;
    }

    Sound texture = LoadSound(filename.c_str());
    _data_sound.insert({filename, texture});
    return texture;
}

Music sounds::load_music(const std::string& filename) {
    auto it = _data_music.find(filename);
    if (it != _data_music.end()) {
        return it->second;
    }

    Music texture = LoadMusicStream(filename.c_str());
    _data_music.insert({filename, texture});
    return texture;
}
