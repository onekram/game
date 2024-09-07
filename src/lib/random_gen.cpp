#include "random_gen.h"

std::random_device random_gen::rd;
std::mt19937 random_gen::gen(random_gen::rd());
