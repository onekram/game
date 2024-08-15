#include "render.h"

namespace render {

auto render_system_factory(Color color) {
    return [color](const movement::position& p) {
        DrawCircle(
            static_cast<int32_t>(std::round(p.x)),
            static_cast<int32_t>(std::round(p.y)),
            global::RADIUS_BALL,
            color
        );
    };
}

void init(flecs::world& world) {
    world.system<movement::position>("RenderSystemPlayer")
        .with<movement::player_tag>()
        .each(render_system_factory(BLACK));
    world.system<movement::position>("RenderSystemEnemy")
        .with<movement::enemy_tag>()
        .each(render_system_factory(YELLOW));
}
} // namespace render
