#pragma once

#include "velocity.h"
#include <functional>

struct position {
    double x, y;

    template <typename Function = std::plus<>>
    void move(const velocity& v, Function func = std::plus<>());

    template <typename FunctionX, typename FunctionY>
    void move(const velocity& v, FunctionX func_x, FunctionY func_y);

    static position generate_random_position(double MIN_X, double MAX_X, double MIN_Y, double MAX_Y);

    friend std::ostream& operator<<(std::ostream& s, const position& p);
};

template <typename Function>
inline void position::move(const velocity& v, Function func) {
    x = func(x, v.x);
    y = func(y, v.y);
}

template <typename FunctionX, typename FunctionY>
inline void position::move(const velocity& v, FunctionX func_x, FunctionY func_y) {
    x = func_x(x, v.x);
    y = func_y(y, v.y);
}
