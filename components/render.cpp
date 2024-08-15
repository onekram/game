#include "render.h"

#include <iostream>

namespace render {

auto render_icon_system_factory(Texture2D texture, Color tint) {
    return [texture, tint](const movement::position& p, const sprite& s) {
        Rectangle source = {s.source_width * s.current_frame, s.source_height, s.source_width, s.source_height};
        Rectangle dest = {p.x, p.y, s.dest_width, s.dest_width};

        DrawTexturePro(texture, source, dest, Vector2{s.dest_width / 2 , s.dest_height / 2}, 0, tint);
    };
}

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

auto render_direction_system_factory(Color color) {
    return [color](const movement::position& p, const mouse_control::mouse& m) {
        float line_length = 30.0f;

        float coord_x = m.x - p.x;
        float coord_y = m.y - p.y;

        float res = std::sqrt(coord_x * coord_x + coord_y * coord_y);
        DrawLineEx(
            Vector2{p.x, p.y},
            Vector2{
                p.x + coord_x * line_length / res,
                p.y + coord_y * line_length / res},
            6,
            color
        );
    };
}

void init(flecs::world& world) {
    init_components<render::sprite>(world);
    Texture2D texture = LoadTexture("../icons/pngegg.png");

    world.system<movement::position, render::sprite>("RenderSystemIcon")
        .each(render::render_icon_system_factory(texture, WHITE));

    world.system<movement::position>("RenderSystemDefault")
        .without<render::sprite>()
        .each(render_system_factory(BLUE));

    world.system<movement::position, mouse_control::mouse>("MouseDirectionSystem")
        .each(render_direction_system_factory(RED));
}
} // namespace render
