#pragma once

#include <random>
#include <type_traits>
#include <vector>

template <typename T>
inline typename std::enable_if<std::is_integral<T>::value, T>::type
get_random(T min, T max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distr(min, max);
    return distr(gen);
}

template <typename T>
inline typename std::enable_if<std::is_floating_point<T>::value, T>::type
get_random(T min, T max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(min, max);
    return dis(gen);
}

template <typename T>
inline T get_random(std::initializer_list<T> list) {
    std::vector<T> vec(list);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distr(0, vec.size() - 1);
    return vec[distr(gen)];
}
