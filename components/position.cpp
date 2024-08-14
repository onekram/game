#include "position.h"

position position::generate_random_position(double MIN_X, double MAX_X, double MIN_Y, double MAX_Y) {
    return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
}

std::ostream& operator<<(std::ostream& s, const position& p) {
    s << "Position: " << p.x << "  " << p.y;
    return s;
}
