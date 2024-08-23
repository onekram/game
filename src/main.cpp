#include "behavior.h"
#include "container.h"
#include "entity_spawn.h"
#include "flecs.h"
#include "global.h"
#include "life.h"
#include "mouse_control.h"
#include "movement.h"
#include "physical_interaction.h"
#include "raylib.h"
#include "render.h"
#include "shooting.h"

void drow(const flecs::world& world) {
    InitWindow(global::WIDTH, global::HEIGHT, "2D Shooter");
    SetTargetFPS(global::FPS);
    InitAudioDevice();

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        world.progress();
        EndDrawing();
    }
    CloseAudioDevice();
    CloseWindow();
}

int main() {
    flecs::world world;
    world.set_target_fps(global::FPS);

    movement::init(world);
    render::init(world);
    life::init(world);
    mouse_control::init(world);
    physical_interaction::init(world);
    behavior::init(world);
    entity_spawn::init(world);

    container::init(world);
    shooting::init(world);
    drow(world);
}
