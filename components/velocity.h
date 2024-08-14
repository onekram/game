#pragma once

#include "random-gen.h"

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

struct velocity {
    double x;
    double y;
    constexpr static const double max = MAX_SPEED;

    static velocity generate_random_velocity();

    friend std::ostream& operator<<(std::ostream& s, const velocity& v);

    void change(double r_x, double r_y);

    void change_vector(double r_x, double r_y);

    void slowdown_x();

    void slowdown_y();

    void reverse_x();

    void reverse_y();
};