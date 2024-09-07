#include "render.h"

#include "behavior.h"
#include "container.h"
#include "distance.h"
#include "init_components.h"
#include "raylib.h"
#include "textures.h"
#include "string_modify.h"

#include <cmath>
#include <iostream>
#include <sstream>

void render::render_icon_system_factory(
    const movement::position& p,
    const sprite& s,
    const sprite_angle* sa
) {
    Rectangle source = {
        s.source_width * (s.current_frame % s.frames_per_line),
        s.source_height * (s.current_frame / s.frames_per_line),
        (s.right_orientation ? 1.0f : -1.0f) * s.source_width,
        s.source_height
    };
    Rectangle dest = {p.x, p.y, s.dest_width, s.dest_width};

    DrawTexturePro(
        textures::load_texture(s.texture),
        source,
        dest,
        Vector2{s.dest_width / 2, s.dest_height / 2},
        sa ? sa->angle : 0,
        WHITE
    );
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

void render::sprite_velocity_system(
    flecs::iter& it,
    std::size_t,
    const movement::velocity& v,
    sprite& st,
    sprite_swap& s
) {
    float speed = std::sqrt(v.x * v.x + v.y * v.y);
    st.right_orientation = v.x > 0;

    if (speed > global::MAX_SPEED / 3) {
        s.elapsed_time += it.delta_time();
        if (s.elapsed_time >= s.frame_swap_time) {
            st.current_frame = (st.current_frame + 1) % st.total_frames;
            s.elapsed_time = 0;
        }
    } else {
        st.current_frame = st.default_frame;
    }
}

void render::sprite_system(flecs::iter& it, std::size_t, sprite& s, sprite_swap& ss) {
    ss.elapsed_time += it.delta_time();
    if (ss.elapsed_time >= ss.frame_swap_time) {
        s.current_frame = (s.current_frame + 1) % s.total_frames;
        ss.elapsed_time = 0;
    }
}

void render::sprite_rotation_at_target_system(
    flecs::entity e,
    const movement::position& p,
    sprite& s
) {
    auto component = e.target<behavior::aiming_at_tag>();
    float tx;
    float ty;
    bool init = false;
    e.world().query_builder<const movement::position>().with(component).build().each(
        [&tx, &ty, p, &init](const movement::position& target_pos) {
            if (!init || get_distance(p.x, p.y, target_pos.x, target_pos.y) <
                             get_distance(tx, ty, p.x, p.y)) {
                tx = target_pos.x;
                ty = target_pos.y;
                init = true;
            }
        }
    );
    if (!init) {
        return;
    }
    float coord_x = tx - p.x;
    float coord_y = ty - p.y;

    float angle = fmod(atan2(coord_y, coord_x) / 3.14 * 180 + 360 + 180 / s.total_frames, 360.0f);
    s.current_frame = fmod((angle / 360) * s.total_frames + s.default_frame, s.total_frames);
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
    const sprite* s
) {
    float shift_y = s ? s->dest_height / 2 : global::RADIUS_BALL;
    float length = 40.0f;
    float k = hp.points / hp.max;
    DrawRectangle(p.x - length / 2, p.y + shift_y, length, 5, BLACK);
    DrawRectangle(
        p.x - length / 2,
        p.y + shift_y,
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
        3,
        0,
        30,
        health_points_color_proportional(hp.points / hp.max)
    );
}

void render::angle_sprite_system(const movement::velocity& v, sprite_angle& sa) {
    sa.angle = atan2(v.y, v.x) / 3.14 * 180;
}

void render::player_inventory_render_system(flecs::entity container) {
    container = container::get_container(container);

    std::int32_t shift = 30;
    container::for_each_item(container, [&shift](flecs::entity item) {
        std::stringstream s;
        s << container::item_name(item);
        Color color;
        if (item.has<container::active_tag>()) {
            color = RED;
            std::int32_t max;
            if (item.get([&max](const container::magazine_size& ms) { max = ms.value; })) {
                std::int32_t count = container::count_items(item);
                s << ' ' << count << '/' << max;
            }
        } else if (!item.has<container::can_hold_tag>()) {
            color = GRAY;
            std::int32_t count = 1;
            item.get([&count](const container::quantity& a) { count = a.value; });
            s << ' ' << count;
        } else {
            color = BLACK;
        }
        s << " - " << modify(container::item_kind(item).name().c_str(), "_tag");
        DrawText(s.str().c_str(), 3, shift, 20, color);
        shift += 20;
    });
}

void render::stage_ammo_render_system(flecs::entity player) {
    auto ammo = container::find_item_active(player);
    std::int32_t count = container::count_items(ammo);
    auto ms = ammo.get<container::magazine_size>();
    if (!ms) {
        return;
    }
    std::size_t i = std::max(5 - count * 5 / ms->value, 0);
    if (i == 5 && count > 0) {
        i = 4;
    }
    float source_width = 93.5;
    float source_height = 133;

    float dest_width = 93.5;
    float dest_height = 133;
    Rectangle source = {source_width * i, source_height, source_width, source_height};
    Rectangle dest = {global::WIDTH * 2 / 3, 10, dest_width, dest_height};

    DrawTexturePro(
        textures::load_texture(PATH "/icons/ammo_stage.png"),
        source,
        dest,
        Vector2{0, 0},
        0,
        WHITE
    );
}

void render::init(flecs::world& world) {
    init_components<sprite, sprite_angle>(world);

    world.system<movement::position, sprite, sprite_angle*>("RenderSystemSprite")
        .kind(flecs::PostUpdate)
        .each(render::render_icon_system_factory);

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

    world.system<movement::velocity, sprite, sprite_swap>("VelocitySpriteSystem")
        .kind(flecs::PostUpdate)
        .each(sprite_velocity_system);

    world
        .system<const movement::position, const life::health_points, const sprite*>(
            "LifePointsSystemRender"
        )
        .kind(flecs::PostUpdate)
        .each(health_points_render_system);

    world.system<const life::health_points>("RenderPlayerHPSystem")
        .with<behavior::player_tag>()
        .each(player_health_points_render_system);

    world.system<const movement::velocity, sprite_angle>("VelocitySpriteAngleSystem")
        .each(angle_sprite_system);

    world.system<sprite, sprite_swap>("SpriteSystem")
        .kind(flecs::PostUpdate)
        .without<movement::velocity>()
        .with<behavior::explosion_tag>()
        .each(sprite_system);

    world.system<>("PlayerInventoryRenderSystem")
        .kind(flecs::PostUpdate)
        .with<behavior::player_tag>()
        .with<container::inventory_tag>(flecs::Wildcard)
        .each(player_inventory_render_system);

    world.system<>("ActiveWeaponMagazinePlayerRender")
        .kind(flecs::PostUpdate)
        .with<behavior::player_tag>()
        .each(stage_ammo_render_system);

    world.system<const movement::position, sprite>("RotationSystemTarget")
        .with<rotation>()
        .with<behavior::aiming_at_tag>(flecs::Wildcard)
        .each(sprite_rotation_at_target_system);
}
