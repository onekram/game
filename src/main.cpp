#include "behavior.h"
#include "flecs.h"
#include "global.h"
#include "life.h"
#include "mouse_control.h"
#include "movement.h"
#include "physical_interaction.h"
#include "raylib.h"
#include "render.h"

#include <cstdint>

flecs::entity
init_enemies(const flecs::world& world, const flecs::entity& player, std::size_t count = 1) {
    auto following_enemy = world.prefab("Entity")
                               .add(flecs::Prefab)
                               .add<behavior::follow_tag>(player)
                               .add<physical_interaction::physical_interaction_tag>();
    for (std::size_t i = 0; i < count; ++i) {
        world.entity()
            .add<behavior::enemy_tag>()
            .set<movement::position>(movement::generate_random_position(
                global::BORDER,
                global::WIDTH - global::BORDER,
                global::BORDER,
                global::HEIGHT - global::BORDER
            ))
            .set<movement::velocity>(movement::generate_random_velocity())
            .set<render::sprite>({0, 3, 2, 0.3f, 0, 32.6f, 47.0f, 17.0f * 3, 25.0f * 3, true})
            .set<life::health_points>({global::ENEMY_LIFE_POINTS, global::ENEMY_LIFE_POINTS})
            .set<life::damage_points>({10})
            .add<behavior::can_damage_tag, behavior::player_tag>()
            .is_a(following_enemy);
    }
    return following_enemy;
}

flecs::entity init_player(const flecs::world& world) {
    return world.entity("Player")
        .add<behavior::player_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<movement::velocity>({0, 0})
        .set<movement::input_movement>(movement::get_default_input())
        .set<render::sprite>({0, 3, 2, 0.2f, 0, 376.0f, 355.0f, 37.0f * 2, 35.0f * 2, true})
        .set<mouse_control::mouse>({0, 0})
        .add<physical_interaction::physical_interaction_tag>()
        .set<life::health_points>({global::PLAYER_LIFE_POINTS, global::PLAYER_LIFE_POINTS})
        .add<behavior::can_damage_tag, behavior::enemy_tag>();
}

flecs::entity init_aid_kid(const flecs::world& world) {
    return world.entity()
        .add<behavior::aid_kit_tag>()
        .set<movement::position>(movement::generate_random_position(
            global::BORDER,
            global::WIDTH - global::BORDER,
            global::BORDER,
            global::HEIGHT - global::BORDER
        ))
        .set<render::sprite>({0, 1, 0, 0.2f, 0, 596.0f, 626.0f, 596.0f / 14, 626.0f / 14, true})
        .add<physical_interaction::physical_interaction_tag>()
        .add<behavior::can_restore_health_tag, behavior::player_tag>()
        .set<behavior::health_restore_points>({500})
        .add<behavior::temporary_tag>();
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
    InitWindow(global::WIDTH, global::HEIGHT, "2D Shooter");
    SetTargetFPS(100);

    flecs::world world;

    init_components<life::health_points>(world);
    movement::init(world);
    render::init(world);

    life::init(world);
    mouse_control::init(world);
    physical_interaction::init(world);
    behavior::init(world);

    auto player = init_player(world);
    init_enemies(world, player, 10);
    init_aid_kid(world);

    drow(world);
}
