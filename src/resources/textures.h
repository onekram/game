#pragma once

#include "raylib.h"

#include <string>
#include <unordered_map>

class textures {
  public:
    static Texture2D load_texture(const std::string& filename);

  private:
    static std::unordered_map<std::string, Texture2D> _data;
};
