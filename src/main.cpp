#include "random-gen.h"

#include <flecs.h>
#include <raylib.h>

#include <cmath>
#include <cstdint>
#include <iostream>

#define WIDTH 800
#define HEIGHT 600

struct screen {
    int32_t width;
    int32_t height;
};

struct velocity {
    double x, y;

    static velocity generate_random_velocity() {
        return {get_random({-0.01, 0.01}), get_random({-0.01, 0.01})};
    };

    friend std::ostream& operator<<(std::ostream& s, const velocity& v) {
        s << "Velocity: " << v.x << "  " << v.y;
        return s;
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

    template <typename Function>
    void move(const velocity& v, Function func) {
        x = func(x, v.x);
        y = func(y, v.y);
    }

    static position generate_random_position(double MIN_X, double MAX_X, double MIN_Y, double MAX_Y) {
        return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
    }

    friend std::ostream& operator<<(std::ostream& s, const position& p) {
        s << "Position: " << p.x << "  " << p.y;
        return s;
    }
};

template <typename Function = std::plus<>>
void move_all_until_edge(const flecs::world& world, Function func = std::plus<>()) {
    int32_t screen_width = world.get<screen>()->width;
    int32_t screen_height = world.get<screen>()->height;

    world.each([&func, &screen_height, &screen_width](position& p, velocity& v) {
        if (p.x < 0 || p.x > screen_width) {
            v.reverse_x();
        }
        if (p.y < 0 || p.y > screen_height) {
            v.reverse_y();
        }
        p.move(v, func);
    });
}

template <typename Function = std::plus<>>
void move_all(const flecs::world& world, Function func = std::plus<>()) {
    world.each([&func](position& p, velocity& v) { p.move(v, func); });
}

void init(const flecs::world& world) {
    InitWindow(world.get<screen>()->width, world.get<screen>()->height, "Flecs and Raylib Example");
}

void init_entities(const flecs::world& world, std::size_t count = 1) {
    for (std::size_t i = 0; i < count; ++i) {
        world.entity()
            .set<position>(
                position::generate_random_position(0, world.get<screen>()->width, 0, world.get<screen>()->width)
            )
            .set<velocity>(velocity::generate_random_velocity());
    }
}

void print_entities(const flecs::world& world) {
    world.each([](position& p, velocity& v) { std::cout << p << '\n' << v << "\n\n"; });
}

void draw_entities(const flecs::world& world) {
    world.each([](position& p) {
        DrawCircle(static_cast<int32_t>(std::round(p.x)), static_cast<int32_t>(std::round(p.y)), 10.0, DARKBLUE);
    });
}

void drow(const flecs::world& world) {
    init(world);
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_entities(world);
        move_all_until_edge(world);

        EndDrawing();
    }
    CloseWindow();
}

int main() {
    flecs::world world;
    world.set<screen>({WIDTH, HEIGHT});
    init_entities(world, 10);
    drow(world);
}
