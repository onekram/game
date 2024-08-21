#include "movement.h"

#include "behavior.h"
#include "render.h"
#include "textures.h"

movement::velocity movement::generate_random_velocity() {
    return {
        get_random(-global::MAX_SPEED / 3, global::MAX_SPEED / 3),
        get_random(-global::MAX_SPEED / 3, global::MAX_SPEED / 3)
    };
}

movement::position
movement::generate_random_position(float MIN_X, float MAX_X, float MIN_Y, float MAX_Y) {
    return {get_random(MIN_X, MAX_X), get_random(MIN_Y, MAX_Y)};
}

movement::input_movement movement::get_default_input() {
    return {false, false, false, false};
}

void movement::change_velocity(velocity& v, float r_x, float r_y, float max_speed) {
    float n_x = v.x + r_x;
    float n_y = v.y + r_y;
    float res = std::sqrt(n_x * n_x + n_y * n_y);
    if (res <= max_speed) {
        v.x = n_x;
        v.y = n_y;
    } else {
        v.x = n_x * (max_speed / res);
        v.y = n_y * (max_speed / res);
    }
}

void movement::velocity_input_system(velocity& v, const input_movement& i) {
    if (i.right == i.left) {
        v.x /= 1.03;
        if (std::fabs(v.x) < global::MAX_SPEED / 100) {
            v.x = 0;
        }
    }
    if (i.up == i.down) {
        v.y /= 1.03;
        if (std::fabs(v.y) < global::MAX_SPEED / 100) {
            v.y = 0;
        }
    }
    change_velocity(
        v,
        (i.right - i.left) * global::MAX_SPEED,
        (i.down - i.up) * global::MAX_SPEED,
        global::MAX_SPEED
    );
}

void movement::move_system(position& p, velocity& v) {
    if (p.x + v.x < global::BORDER) {
        p.x = global::BORDER;
        v.x = 0;
    }
    if (p.x + v.x > global::WIDTH - global::BORDER) {
        p.x = global::WIDTH - global::BORDER;
        v.x = 0;
    }
    if (p.y + v.y < global::BORDER) {
        p.y = global::BORDER;
        v.y = 0;
    }
    if (p.y + v.y > global::HEIGHT - global::BORDER) {
        p.y = global::HEIGHT - global::BORDER;
        v.y = 0;
    }
    p.x += v.x;
    p.y += v.y;
}

void movement::move_bounce_system(position& p, velocity& v) {
    if (p.x < global::BORDER || p.x > global::WIDTH - global::BORDER) {
        v.x = -v.x;
    }
    if (p.y < global::BORDER || p.y > global::HEIGHT - global::BORDER) {
        v.y = -v.y;
    }
    p.x += v.x;
    p.y += v.y;
}

void movement::velocity_follow_player_system(flecs::entity e, const position& p, velocity& v) {
    auto target_pos = e.target<behavior::follow_tag>().get<position>();
    change_velocity(
        v,
        (target_pos->x > p.x ? 1.0f : -1.0f) * global::MAX_SPEED / 10,
        (target_pos->y > p.y ? 1.0f : -1.0f) * global::MAX_SPEED / 10,
        global::MAX_SPEED / 2
    );
}

void movement::input_system(input_movement& input) {
    input.up = IsKeyDown(KEY_W);
    input.down = IsKeyDown(KEY_S);
    input.left = IsKeyDown(KEY_A);
    input.right = IsKeyDown(KEY_D);
}

void movement::shoot_system(
    flecs::iter& it,
    std::size_t,
    const position& p,
    const mouse_control::mouse& m
) {
    if (m.pressed) {
        float length = global::BULLET_VELOCITY;
        float v_x = m.x - p.x;
        float v_y = m.y - p.y;
        float res = std::sqrt(v_x * v_x + v_y * v_y);
        it.world()
            .entity()
            .set<position>({p.x, p.y})
            .set<velocity>({v_x * length / res, v_y * length / res})
            .set<life::damage_points>({10})
            .set<life::life_time>({1})
            .add<behavior::bullet_tag>()
            .add<behavior::can_damage_tag, behavior::enemy_tag>()
            .add<behavior::can_damage_tag, behavior::tnt_barrel_tag>()
            .add<life::temporary_tag>()
            .set<render::sprite>(
                {0,
                 1,
                 0,
                 0,
                 0,
                 748,
                 365,
                 748 / 27,
                 365 / 27,
                 true,
                 textures::load_texture("../icons/bullet.png")}
            )
            .add<render::sprite_angle>()
            .set<physical_interaction::interaction_radius>({3});
    }
}

void movement::init(flecs::world& world) {
    init_components<position, velocity, input_movement>(world);

    init_components<behavior::enemy_tag, behavior::player_tag, behavior::follow_tag>(world);

    world.system<position, velocity>("MovementSystem").each(move_system);

    world.system<velocity, input_movement>("VelocityControlSystem")
        .kind(flecs::PreUpdate)
        .each(velocity_input_system);

    world.system<position, velocity>("VelocitySystemFollowingEnemy")
        .with<behavior::enemy_tag>()
        .with<behavior::follow_tag>(flecs::Wildcard)
        .each(velocity_follow_player_system);

    world.system<input_movement>("InputMovementSystem").each(input_system);

    world.system<position, mouse_control::mouse>("ShootSystemPlayer").each(shoot_system);
}
