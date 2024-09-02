#include "background_music.h"

#include "init_components.h"
#include "render.h"
#include "sounds.h"

void background_music::play_music_system(music& m) {
    PlayMusicStream(sounds::load_music(m.filename));
}

void background_music::update_music_system(music& m) {
    UpdateMusicStream(sounds::load_music(m.filename));
}

void background_music::init(flecs::world& world) {
    world.set<music>({"../sounds/Enigma-Long-Version.mp3"});

    world.system<music>("BackgroundMusicPlayStartSystem")
        .kind(flecs::OnStart)
        .term_at(0)
        .singleton()
        .each(play_music_system);

    world.system<music>("BackgroundMusicUpdateSystem")
        .kind(flecs::PostUpdate)
        .term_at(0)
        .singleton()
        .each(update_music_system);
}
