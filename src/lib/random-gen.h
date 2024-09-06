#pragma once

#include <random>
#include <type_traits>
#include <vector>
class random_gen {
public:
    template <typename T>
    static typename std::enable_if<std::is_integral<T>::value, T>::type get_random(T min, T max) {
        std::uniform_int_distribution<> distr(min, max);
        return distr(gen);
    }

    template <typename T>
    static typename std::enable_if<std::is_floating_point<T>::value, T>::type get_random(T min, T max) {
        std::uniform_real_distribution<> dis(min, max);
        return dis(gen);
    }

    template <typename T>
    static T get_random(std::initializer_list<T> list) {
        std::vector<T> vec(list);
        std::uniform_int_distribution<std::size_t> distr(0, vec.size() - 1);
        return vec[distr(gen)];
    }

private:
    static std::random_device rd;
    static std::mt19937 gen;
};
