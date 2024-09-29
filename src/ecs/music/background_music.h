#pragma once
#include "flecs.h"

#include <string>

namespace background_music {
struct music {
    std::string filename;
};

void play_music_system(music& m);
void update_music_system(music& m);

void init(flecs::world& world);
} // namespace background_music
