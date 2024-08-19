#include "mouse_control.h"

void mouse_control::mouse_control_system(mouse& m) {
    m.x = GetMousePosition().x;
    m.y = GetMousePosition().y;

    m.pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void mouse_control::init(flecs::world& world) {
    init_components<mouse>(world);

    world.system<mouse>().each(mouse_control_system);
}
