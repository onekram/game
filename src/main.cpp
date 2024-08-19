#include "behavior.h"
#include "entity_spawn.h"
#include "flecs.h"
#include "global.h"
#include "life.h"
#include "mouse_control.h"
#include "movement.h"
#include "physical_interaction.h"
#include "raylib.h"
#include "render.h"

#include <cstdint>

void drow(const flecs::world& world) {
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        world.progress();
        EndDrawing();
    }
    CloseWindow();
}

int main() {
    InitWindow(global::WIDTH, global::HEIGHT, "2D Shooter");
    SetTargetFPS(100);

    flecs::world world;

    movement::init(world);
    render::init(world);
    life::init(world);
    mouse_control::init(world);
    physical_interaction::init(world);
    behavior::init(world);
    entity_spawn::init(world);

    drow(world);
}
