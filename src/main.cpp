#include <flecs.h>
#include <raylib.h>

#include <cstdint>
#include <iostream>

const int screenWidth = 800;
const int screenHeight = 600;

struct velocity {
    float x, y;

};

struct position {
    float x, y;

    template <typename Function>
    void move(const velocity& v, Function func) {
        x = func(x, v.x);
        y = func(y, v.y);
    }

    void move_vector(const velocity& v) {
        move(v, std::plus<>());
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
    InitWindow(screenWidth, screenHeight, "Flecs and Raylib Example");
}

void init_entities(const flecs::world& world, std::size_t count = 1) {
    for (std::size_t i = 0; i < count; ++i) {
        auto e = world.entity();
    }
    world.each([](flecs::entity& e) {
        e.set<position>({10, 20});
        e.set<velocity>({1, 1});
    });
}

// Render system
void render(ecs_iter_t* it) {}

int main() {
    // init();
    flecs::world world;
}