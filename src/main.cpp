#include "velocity.h"
#include "position.h"
#include "input.h"

#include <flecs.h>
#include <raylib.h>

#include <cmath>
#include <cstdint>
#include <iostream>

#define WIDTH 1300
#define HEIGHT 1000

#define MAX_SPEED 0.09
#define BORDER 7.0

#define RADIUS_BALL 10.0

struct player_tag {};

struct enemy_tag {};

struct follow_tag {};

template <typename... Types>
void init_components(const flecs::world& world) {
    (world.component<Types>(), ...);
}

struct movement {
    movement(flecs::world& world) {
        init_components<position, velocity, input>(world);
    }
};

struct entity_tags {
    entity_tags(flecs::world& world) {
        init_components<player_tag, enemy_tag>(world);
    }
};

struct behaivour {
    behaivour(flecs::world& world) {
        init_components<follow_tag>(world);
    }
};

void input_system(input& input) {
    input.up = IsKeyDown(KEY_W);
    input.down = IsKeyDown(KEY_S);
    input.left = IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_D);
}

void velocity_input_system(velocity& v, const input& i) {
    if (i.right == i.left && i.down == i.up) {
        v.slowdown_x();
        v.slowdown_y();
    } else {
        v.change((i.right - i.left) * velocity::max / 1000, (i.down - i.up) * velocity::max / 1000);
    }
}

void move_system(position& p, velocity& v) {
    if (p.x + v.x < BORDER) {
        p.x = BORDER;
        v.x = 0;
    }
    if (p.x + v.x > WIDTH - BORDER) {
        p.x = WIDTH - BORDER;
        v.x = 0;
    }
    if (p.y + v.y < BORDER) {
        p.y = BORDER;
        v.y = 0;
    }
    if (p.y + v.y > HEIGHT - BORDER) {
        p.y = HEIGHT - BORDER;
        v.y = 0;
    }
    p.move(v);
}

void move_bounce_system(position& p, velocity& v) {
    if (p.x < BORDER || p.x > WIDTH - BORDER) {
        v.reverse_x();
    }
    if (p.y < BORDER || p.y > HEIGHT - BORDER) {
        v.reverse_y();
    }
    p.move(v);
}

void velocity_follow_player_system(flecs::entity e, const position& p, velocity& v) {
    auto target_pos = e.target<follow_tag>().get<position>();
    v.change_vector(target_pos->x - p.x, target_pos->y - p.y);
}

void repulsion(position& pos1, position& pos2, double dist, double k) {
    double dx = pos1.x - pos2.x;
    double dy = pos1.y - pos2.y;
    double distance = std::sqrt(dx * dx + dy * dy);

    if (distance < dist) {
        double force = k / (distance + 0.1);
        pos1.x += (dx / distance) * force;
        pos1.y += (dy / distance) * force;
        pos2.x -= (dx / distance) * force;
        pos2.y -= (dy / distance) * force;
    }
}

auto render_system_factory(Color color) {
    return [color](const position& p) {
        DrawCircle(static_cast<int32_t>(std::round(p.x)), static_cast<int32_t>(std::round(p.y)), RADIUS_BALL, color);
    };
}

flecs::entity init_enemies(const flecs::world& world, const flecs::entity& player, std::size_t count = 1) {
    auto following_enemy = world.prefab("Entity").add(flecs::Prefab).add<follow_tag>(player);
    for (std::size_t i = 0; i < count; ++i) {
        world.entity()
            .add<enemy_tag>()
            .set<position>(position::generate_random_position(
                BORDER,
                WIDTH - BORDER,
                BORDER,
                HEIGHT- BORDER
            ))
            .set<velocity>(velocity::generate_random_velocity())
            .is_a(following_enemy);
    }
    return following_enemy;
}

flecs::entity init_player(const flecs::world& world) {
    return world.entity("Player")
        .add<player_tag>()
        .set<position>(position::generate_random_position(
            BORDER,
            WIDTH- BORDER,
            BORDER,
            HEIGHT - BORDER
        ))
        .set<velocity>({0, 0})
        .set<input>(input::get_default_input());
}

void init_system(const flecs::world& world) {
    world.system<position, velocity>("MovementSystem").each(move_system);

    world.system<velocity, input>("VelocityControlSystemPlayer").with<player_tag>().each(velocity_input_system);

    world.system<position, velocity>("VelocitySystemFollowingEnemy")
        .with<enemy_tag>()
        .with<follow_tag>(flecs::Wildcard)
        .each(velocity_follow_player_system);

    world.system<input>("InputSystemPlayer").with<player_tag>().each(input_system);

    world.system<position>("RenderSystemPlayer").with<player_tag>().each(render_system_factory(BLACK));

    world.system<position>("RenderSystemEnemy").with<enemy_tag>().each(render_system_factory(YELLOW));

    world.system<position>("RepulsionEntitiesSystem").each([&world](flecs::entity e1, position& pos1) {
        world.each<position>([&](flecs::entity e2, position& pos2) {
            if (e1.id() != e2.id()) {
                repulsion(pos1, pos2, RADIUS_BALL + 2, 5);
            }
        });
    });
}

void init_log_system(const flecs::world& world) {
    world.system<>("FollowPrintSystem").with<follow_tag>(flecs::Wildcard).interval(1.0).each([](flecs::entity e) {
        std::cout << "FOLLOW TARGET" << *e.target<follow_tag>().get<position>() << std::endl;
    });
    world.system<position>("PositionPrintSystemPlayer").with<player_tag>().interval(1.0).each([](position& p) {
        std::cout << "PLAYER " << p << std::endl;
    });
}

void drow(const flecs::world& world) {
    InitWindow(WIDTH, HEIGHT, "Flecs and Raylib Example");
    SetWindowState(FLAG_FULLSCREEN_MODE);

    while (!WindowShouldClose()) {
        BeginDrawing();
        world.progress();

        ClearBackground(RAYWHITE);


        EndDrawing();
    }
    CloseWindow();
}

void print_entities(const flecs::world& world) {
    world.each([](position& p, velocity& v) { std::cout << p << '\n' << v << "\n\n"; });
}

int main() {
    flecs::world world;

    world.import <movement>();
    world.import <behaivour>();
    world.import <entity_tags>();

    init_system(world);
    // init_log_system(world);

    auto player = init_player(world);
    init_enemies(world, player, 10);

    drow(world);
}
