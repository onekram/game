#include "render.h"

#include "behavior.h"
#include "init_components.h"
#include "raylib.h"

#include <cmath>
#include <iostream>
#include <sstream>

auto render::render_icon_system_factory(Color tint) {
    return [tint](const movement::position& p, const sprite& s, const sprite_angle* sa) {
        Rectangle source = {
            s.source_width * s.current_frame,
            s.source_height,
            (s.right_orientation ? 1.0f : -1.0f) * s.source_width,
            s.source_height
        };
        Rectangle dest = {p.x, p.y, s.dest_width, s.dest_width};

        DrawTexturePro(
            s.texture,
            source,
            dest,
            Vector2{s.dest_width / 2, s.dest_height / 2},
            sa ? sa->angle : 0,
            tint
        );
    };
}

auto render::render_system_factory(Color color) {
    return [color](const movement::position& p, const physical_interaction::repulsion_radius* rr) {
        DrawCircle(
            static_cast<int32_t>(std::round(p.x)),
            static_cast<int32_t>(std::round(p.y)),
            rr ? rr->radius : global::RADIUS_BALL,
            color
        );
    };
}

auto render::render_direction_system_factory(Color color) {
    return [color](const movement::position& p, const mouse_control::mouse& m) {
        float line_length = 30.0f;

        float coord_x = m.x - p.x;
        float coord_y = m.y - p.y;

        float res = std::sqrt(coord_x * coord_x + coord_y * coord_y);
        DrawLineEx(
            Vector2{p.x, p.y},
            Vector2{p.x + coord_x * line_length / res, p.y + coord_y * line_length / res},
            6,
            color
        );
    };
}

void render::sprite_system(flecs::iter& it, std::size_t, const movement::velocity& v, sprite& s) {
    float speed = std::sqrt(v.x * v.x + v.y * v.y);
    s.right_orientation = v.x > 0;

    if (speed > global::MAX_SPEED / 3) {
        s.elapsed_time += it.delta_time();
        if (s.elapsed_time >= s.frame_swap_time) {
            s.current_frame = (s.current_frame + 1) % s.total_frames;
            s.elapsed_time = 0;
        }
    } else {
        s.current_frame = s.default_frame;
    }
}

Color render::health_points_color_proportional(float k) {
    if (k >= 0.5f) {
        return {
            static_cast<unsigned char>(255 * (1 - k) * 2),
            static_cast<unsigned char>(255),
            0,
            255
        };
    }
    return {static_cast<unsigned char>(255), static_cast<unsigned char>(255 * k), 0, 255};
}

void render::health_points_render_system(
    const movement::position& p,
    const life::health_points& hp,
    const sprite& s
) {
    float length = 40.0f;
    float k = hp.points / hp.max;
    DrawRectangle(p.x - length / 2, p.y + s.dest_height / 2, length, 5, BLACK);
    DrawRectangle(
        p.x - length / 2,
        p.y + s.dest_height / 2,
        length * k,
        5,
        health_points_color_proportional(k)
    );
}

void render::player_health_points_render_system(const life::health_points& hp) {
    std::stringstream to_print;
    to_print << "HP: " << hp.points << " / " << hp.max;
    DrawText(
        to_print.str().c_str(),
        0,
        0,
        20,
        health_points_color_proportional(hp.points / hp.max)
    );
}

void render::angle_sprite_system(const movement::velocity& v, sprite_angle& sa) {
    sa.angle = std::atan(v.y / v.x) / 3.14 * 180;
}

void render::init(flecs::world& world) {
    init_components<sprite, sprite_angle>(world);

    world.system<movement::position, sprite, sprite_angle*>("RenderSystemSprite")
        .kind(flecs::PostUpdate)
        .each(render::render_icon_system_factory(WHITE));

    world
        .system<movement::position, const physical_interaction::repulsion_radius*>(
            "RenderSystemDefault"
        )
        .kind(flecs::PostUpdate)
        .without<sprite>()
        .each(render_system_factory(BLUE));

    world.system<movement::position, mouse_control::mouse>("MouseDirectionSystem")
        .kind(flecs::PostUpdate)
        .each(render_direction_system_factory(RED));

    world.system<movement::velocity, sprite>("VelocitySpriteSystem")
        .kind(flecs::PostUpdate)
        .each(sprite_system);

    world
        .system<const movement::position, const life::health_points, const sprite>(
            "LifePointsSystemRender"
        )
        .kind(flecs::PostUpdate)
        .each(health_points_render_system);

    world.system<const life::health_points>("RenderPlayerHPSystem")
        .with<behavior::player_tag>()
        .each(player_health_points_render_system);

    world.system<const movement::velocity, sprite_angle>("VelocitySpriteAngleSystem")
        .each(angle_sprite_system);
}
