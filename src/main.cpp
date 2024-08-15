#include "global.h"
#include "mouse_control.h"
#include "movement.h"
#include "render.h"

#include <flecs.h>
#include <raylib.h>

#include <cstdint>

flecs::entity init_enemies(
    const flecs::world& world,
    const flecs::entity& player,
    std::size_t count = 1
) {
    auto following_enemy = world.prefab("Entity")
                               .add(flecs::Prefab)
                               .add<movement::follow_tag>(player);
    for (std::size_t i = 0; i < count; ++i) {
        world.entity()
            .add<movement::enemy_tag>()
            .set<movement::position>(movement::generate_random_position(
                global::BORDER,
                global::WIDTH - global::BORDER,
                global::BORDER,
                global::HEIGHT - global::BORDER
            ))
            .set<movement::velocity>(movement::generate_random_velocity())
            .is_a(following_enemy);
    }
    return following_enemy;
}

flecs::entity init_player(const flecs::world& world) {
    return world.entity("Player")
        .add<movement::player_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<movement::velocity>({0, 0})
        .set<movement::input_movement>(movement::get_default_input())
        .set<render::sprite>({0, 3,2,  0.1f, 0, 376, 355, 37 * 2, 35 * 2})
        .set<mouse_control::mouse>({0, 0});
}

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
    InitWindow(global::WIDTH, global::HEIGHT, "Flecs and Raylib Example");
    SetTargetFPS(1000);

    flecs::world world;

    movement::init(world);
    render::init(world);
    mouse_control::init(world);

    auto player = init_player(world);
    init_enemies(world, player, 10);

    drow(world);
}
