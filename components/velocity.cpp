#include "velocity.h"

velocity velocity::generate_random_velocity() {
    return {get_random(-max / 3, max / 3), get_random(-max / 3, max / 3)};

}

std::ostream& operator<<(std::ostream& s, const velocity& v) {
    s << "Velocity: " << v.x << "  " << v.y;
    return s;
}

void velocity::change(double r_x, double r_y) {
    double n_x = x + r_x;
    double n_y = y + r_y;
    double res = std::sqrt(n_x * n_x + n_y * n_y);
    if (res <= max) {
        x = n_x;
        y = n_y;
    } else {
        x = n_x * (max / res);
        y = n_y * (max / res);
    }
}

void velocity::change_vector(double r_x, double r_y) {
    double n_x = x + r_x;
    double n_y = y + r_y;
    double old = std::sqrt(x * x + y * y);

    double res = std::sqrt(n_x * n_x + n_y * n_y);
    if (res <= old) {
        x = n_x;
        y = n_y;
    } else {
        x = n_x * (old / res);
        y = n_y * (old / res);
    }
}

void velocity::slowdown_x() {
    x /= 1.001;
    if (std::fabs(x) < max / 10) {
        x = 0;
    }
}

void velocity::slowdown_y() {
    y /= 1.001;
    if (std::fabs(y) < max / 10) {
        y = 0;
    }
}

void velocity::reverse_x() {
    x = -x;
}

void velocity::reverse_y() {
    y = -y;
}