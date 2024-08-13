#include "random-gen.h"

#include <flecs.h>
#include <raylib.h>

#include <cmath>
#include <cstdint>
#include <iostream>

const uint32_t screen_width = 800;
const uint32_t screen_height = 600;

struct velocity {
    double x, y;

    static velocity generate_random_velocity() {
        return {get_random({-0.01, 0.01}), get_random({-0.01, 0.01})};
    };

    friend std::ostream& operator<<(std::ostream& s, const velocity& v) {
        s << "Velocity: " << v.x << "  " << v.y;
        return s;
    }
};

struct position {
    double x, y;

    template <typename Function>
    void move(const velocity& v, Function func) {
        x = func(x, v.x);
        y = func(y, v.y);
    }

    void move_vector(const velocity& v) {
        move(v, std::plus<>());
    }

    static position generate_random_position(double MIN_X, double MAX_X, double MIN_Y, double MAX_Y) {
        return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
    }

    friend std::ostream& operator<<(std::ostream& s, const position& p) {
        s << "Position: " << p.x << "  " << p.y;
        return s;
    }
};

void move_all(const flecs::world& world) {
    world.each([](position& p, velocity& v) { p.move_vector(v); });
}

template <typename Function>
void move_all(const flecs::world& world, Function func) {
    world.each([&func](position& p, velocity& v) { p.move(v, func); });
}

void init() {
    InitWindow(screen_width, screen_height, "Flecs and Raylib Example");
}

void init_entities(const flecs::world& world, std::size_t count = 1) {
    for (std::size_t i = 0; i < count; ++i) {
        world.entity()
            .set<position>(position::generate_random_position(0, screen_width, 0, screen_height))
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
    init();
    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_entities(world);
        move_all(world);

        EndDrawing();
    }
    CloseWindow();
}

int main() {
    flecs::world world;
    init_entities(world, 10);
//    print_entities(world);
    drow(world);
}
