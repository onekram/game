#include "background_music.h"
#include "behavior.h"
#include "container.h"
#include "entity_spawn.h"
#include "flecs.h"
#include "global.h"
#include "life.h"
#include "load_prefabs.h"
#include "mouse_control.h"
#include "movement.h"
#include "physical_interaction.h"
#include "pugixml.hpp"
#include "raylib.h"
#include "render.h"
#include "shooting.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
static flecs::world world;

void update_frame() {
    BeginDrawing();

    ClearBackground(RAYWHITE);
    world.progress();
    EndDrawing();
}

void drow() {
    InitWindow(global::WIDTH, global::HEIGHT, "2D Shooter");
    InitAudioDevice();
// SetWindowState(FLAG_FULLSCREEN_MODE);
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(update_frame, 100, 1);
#else
    SetTargetFPS(global::FPS);
    while (!WindowShouldClose()) {
        update_frame();
    }
#endif

    CloseAudioDevice();
    CloseWindow();
}

int main() {
    world.set_target_fps(global::FPS);

    load_prefabs::init(world);

    background_music::init(world);

    movement::init(world);
    render::init(world);
    life::init(world);
    mouse_control::init(world);
    physical_interaction::init(world);
    behavior::init(world);
    entity_spawn::init(world);

    container::init(world);
    shooting::init(world);
    drow();
}
