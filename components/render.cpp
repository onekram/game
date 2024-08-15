#include "render.h"

#include <iostream>

namespace render {

auto render_icon_system_factory(float scale, Color tint) {
    return [&scale, &tint](const movement::position& p, const icon_type& i_t) {
        // if (i_t.stand && i_t.right) {
        //     Texture2D stand_right = LoadTexture("../icons/stand_right.png");
        //     DrawTextureEx(stand_right, (Vector2){p.x, p.y}, 0.0f, scale, tint);
        // } else if (i_t.stand && !i_t.right) {
        //     Texture2D stand_left = LoadTexture("../icons/stand_left.png");
        //     DrawTextureEx(stand_left, (Vector2){p.x, p.y}, 0.0f, scale, tint);
        // } else if (!i_t.stand && i_t.right) {
        //     Texture2D running_right = LoadTexture("../icons/running_right.png");
        //     DrawTextureEx(running_right, (Vector2){p.x, p.y}, 0.0f, scale, tint);
        // } else {
        //     Texture2D running_left = LoadTexture("../icons/running_left.png");
        //     DrawTextureEx(running_left, (Vector2){p.x, p.y}, 0.0f, scale, tint);
        // }

        float height = 17;
        float width = 20;
        Vector2 v1, v2, v3;
        if (i_t.stand && i_t.right) {
            v1 = {p.x, p.y - height / 2};
            v2 = {p.x, p.y + height / 2};
            v3 = {p.x + width, p.y};
        } else if (i_t.stand && !i_t.right) {
            v1 = {p.x, p.y - height / 2};
            v2 = {p.x - width, p.y};
            v3 = {p.x, p.y + height / 2};
        } else if (!i_t.stand && i_t.right) {
            v1 = {p.x, p.y - height / 2};
            v2 = {p.x, p.y + height / 2};
            v3 = {p.x + width * 2, p.y};
        } else {
            v1 = {p.x, p.y - height / 2};
            v2 = {p.x - width * 2, p.y};
            v3 = {p.x, p.y + height / 2};
        }
        DrawTriangle(v1, v2, v3, RED);
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

void init(flecs::world& world) {
    init_components<icon_type>(world);
    world.system<movement::position, render::icon_type>("RenderSystemPlayer")
        .with<movement::player_tag>()
        .each(render_icon_system_factory(0.5f, RED));

    world.system<movement::position>("RenderSystemEnemy")
        .with<movement::enemy_tag>()
        .each(render_system_factory(YELLOW));
}
} // namespace render
