#pragma once

#include <cmath>

template <typename T>
inline T get_distance(T x, T y) {
    return std::sqrt(x * x + y * y);
}

template <typename T>
inline T get_distance(T x1, T y1, T x2, T y2) {
    T dx = x1 - x2;
    T dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}
