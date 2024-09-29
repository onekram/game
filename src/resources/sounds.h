#pragma once

#include "raylib.h"

#include <string>
#include <unordered_map>

class sounds {
  public:
    static Sound load_sound(const std::string& filename);

    static Music load_music(const std::string& filename);

  private:
    static std::unordered_map<std::string, Sound> _data_sound;
    static std::unordered_map<std::string, Music> _data_music;
};
