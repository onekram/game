#include <random>
#include <vector>

template <typename T>
static T get_random(T min, T max) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> distr(min, max);

    return (distr(gen) == 0) ? -1 : 1;
}

template <typename T>
T get_random_from_list(const std::vector<T>& list) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<T> distr(0, list.size() - 1);

    return (distr(gen) == 0) ? -1 : 1;
}
