#pragma once
#include "raylib.h"

#include <string>
#include <unordered_map>

class sounds {
  public:
    static Sound load_sound(const std::string& filename);

  private:
    static std::unordered_map<std::string, Sound> _data;
};
