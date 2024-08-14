#include "random-gen.h"

#include <flecs.h>
#include <raylib.h>

#include <cmath>
#include <cstdint>
#include <iostream>

#define WIDTH 1300
#define HEIGHT 1000

#define BORDER 13

struct player_tag {};

struct enemy_tag {};

struct screen {
    int32_t width;
    int32_t height;
};

struct velocity {
    double x, y;
    constexpr static const double max = 0.07;

    static velocity generate_random_velocity() {
        return {get_random(-max, max), get_random(-max, max)};
    };

    friend std::ostream& operator<<(std::ostream& s, const velocity& v) {
        s << "Velocity: " << v.x << "  " << v.y;
        return s;
    }

    void change(double r_x, double r_y) {
        double n_x = x + r_x;
        double n_y = y + r_y;
        double res = std::sqrt(n_x * n_x + n_y * n_y);
        if (res <= max) {
            x = n_x;
            y = n_y;
        } else {
            x = n_x * (max / res);
            y = n_y * (max / res);
        }
    }

    void slowdown_x() {
        x /= 1.001;
        if (std::fabs(x) < max / 10) {
            x = 0;
        }
    }

    void slowdown_y() {
        y /= 1.001;
        if (std::fabs(y) < max / 10) {
            y = 0;
        }
    }

    void reverse() {
        x = -x;
        y = -y;
    }

    void reverse_x() {
        x = -x;
    }

    void reverse_y() {
        y = -y;
    }
};

struct position {
    double x, y;

    template <typename Function = std::plus<>>
    void move(const velocity& v, Function func = std::plus<>()) {
        x = func(x, v.x);
        y = func(y, v.y);
    }

    template <typename FunctionX, typename FunctionY>
    void move(const velocity& v, FunctionX func_x, FunctionY func_y) {
        x = func_x(x, v.x);
        y = func_y(y, v.y);
    }

    static position generate_random_position(double MIN_X, double MAX_X, double MIN_Y, double MAX_Y) {
        return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
    }

    friend std::ostream& operator<<(std::ostream& s, const position& p) {
        s << "Position: " << p.x << "  " << p.y;
        return s;
    }
};

struct input {
    bool up;
    bool down;
    bool left;
    bool right;

    static input get_default_input() {
        return {false, false, false, false};
    }
};

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

void input_system(input& input) {
    input.up = IsKeyDown(KEY_W);
    input.down = IsKeyDown(KEY_S);
    input.left = IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_D);
}

void velocity_control_system(velocity& v, const input& i) {
    if (i.right == i.left && i.down == i.up) {
        v.slowdown_x();
        v.slowdown_y();
    } else {
        v.change((i.right - i.left) * velocity::max / 1000, (i.down - i.up) * velocity::max / 1000);
    }
}

void move_system(position& p, const velocity& v) {
    p.move(v);
}

void move_system_random(position& p, velocity& v) {
    if (p.x < BORDER || p.x > WIDTH - BORDER) {
        v.reverse_x();
    }
    if (p.y < BORDER || p.y > HEIGHT - BORDER) {
        v.reverse_y();
    }
    p.move(v);
}

auto render_system_factory(Color color) {
    return [color](const position& p) {
        DrawCircle(static_cast<int32_t>(std::round(p.x)), static_cast<int32_t>(std::round(p.y)), 10.0, color);
    };
}

void init_enemies(const flecs::world& world, std::size_t count = 1) {
    for (std::size_t i = 0; i < count; ++i) {
        world.entity()
            .add<enemy_tag>()
            .set<position>(position::generate_random_position(
                BORDER,
                world.get<screen>()->width - BORDER,
                BORDER,
                world.get<screen>()->width - BORDER
            ))
            .set<velocity>(velocity::generate_random_velocity());
    }
}

void init_player(const flecs::world& world) {
    world.entity("Player")
        .add<player_tag>()
        .set<position>(position::generate_random_position(
            BORDER,
            world.get<screen>()->width - BORDER,
            BORDER,
            world.get<screen>()->width - BORDER
        ))
        .set<velocity>({0.01f, 0.01f})
        .set<input>(input::get_default_input());
}

void init_system(const flecs::world& world) {
    world.system<position, velocity>("MovementSystemPlayer").with<player_tag>().each(move_system);

    world.system<velocity, input>("VelocityControlSystemPlayer").with<player_tag>().each(velocity_control_system);

    world.system<position, velocity>("MovementSystemEnemy").with<enemy_tag>().each(move_system_random);

    world.system<input>("InputSystem").each(input_system);

    world.system<position>("RenderSystemPlayer").with<player_tag>().each(render_system_factory(BLACK));

    world.system<position>("RenderSystemEnemy").with<enemy_tag>().each(render_system_factory(YELLOW));
}

void drow(const flecs::world& world) {
    InitWindow(world.get<screen>()->width, world.get<screen>()->height, "Flecs and Raylib Example");
    while (!WindowShouldClose()) {
        world.progress();
        BeginDrawing();

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
    world.import <entity_tags>();

    world.set<screen>({WIDTH, HEIGHT});

    init_system(world);

    init_enemies(world, 10);
    init_player(world);

    print_entities(world);

    drow(world);
}
