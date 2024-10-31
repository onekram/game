#include "behavior/behavior.h"
#include "behavior/life.h"
#include "entity/load_prefabs.h"
#include "flecs.h"
#include "gameplay/mouse_control.h"
#include "gameplay/movement.h"
#include "gameplay/shooting.h"
#include "globals/globals.h"
#include "inventory/container.h"
#include "music/background_music.h"
#include "physics/physical_interaction.h"
#include "raylib.h"
#include "render/render.h"
#include "resources/entity_from_xml.h"

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

    background_music::init(world);

    movement::init(world);
    render::init(world);
    life::init(world);
    mouse_control::init(world);
    physical_interaction::init(world);
    behavior::init(world);

    container::init(world);
    shooting::init(world);

    load_prefabs::init(world);
    entity_from_xml::load_levels(world, std::string(global::XML_PATH));

    drow();
}
