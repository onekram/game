#include "mouse_control.h"

namespace mouse_control {
void mouse_control_system(mouse& m) {
    m.x = GetMousePosition().x;
    m.y = GetMousePosition().y;

    m.pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void init(flecs::world& world) {
    init_components<mouse>(world);

    world.system<mouse>().each(mouse_control_system);
}
} // namespace mouse_control
