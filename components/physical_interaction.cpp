#include "physical_interaction.h"

#include <iostream>

void physical_interaction::repulsion_system(
    flecs::iter& it,
    std::size_t i,
    movement::position& p1
) {
    auto q = it.world().query_builder<movement::position>().with<can_repulsion_tag>().build();
    flecs::entity e1 = it.entity(i);

    q.each([&](flecs::entity e2, movement::position& p2) {
        if (e1 != e2) {
            float dx = p1.x - p2.x;
            float dy = p1.y - p2.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < global::RADIUS_BALL * 2) {
                float force = global::FORCE / (distance + 0.1);
                p1.x += (dx / distance) * force;
                p1.y += (dy / distance) * force;
                p2.x -= (dx / distance) * force;
                p2.y -= (dy / distance) * force;
            }
        }
    });
}

void physical_interaction::interaction_handle_system(
    flecs::iter& it,
    std::size_t i,
    movement::position& p1
) {
    auto q = it.world().query_builder<movement::position>().with<can_interaction_tag>().build();
    flecs::entity e1 = it.entity(i);

    q.each([&](flecs::entity e2, movement::position& p2) {
        if (e1 != e2) {
            float dx = p1.x - p2.x;
            float dy = p1.y - p2.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < global::RADIUS_BALL * 2) {
                e1.add<interaction_tag>(e2);
            } else {
                e1.remove<interaction_tag>(e2);
            }
        }
    });
}

void physical_interaction::init(flecs::world& world) {
    init_components<can_interaction_tag>(world);

    world.system<movement::position>("InteractionSystem")
        .kind(flecs::PreUpdate)
        .with<can_interaction_tag>()
        .each(interaction_handle_system);

    world.system<movement::position>("RepulsionSystem")
        .kind(flecs::PreUpdate)
        .with<can_repulsion_tag>()
        .each(repulsion_system);
}
