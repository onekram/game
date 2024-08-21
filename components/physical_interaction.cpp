#include "physical_interaction.h"

#include <iostream>

void physical_interaction::repulsion_system(
    flecs::iter& it,
    std::size_t i,
    movement::position& p1,
    const repulsion_radius& rr1
) {
    auto q = it.world()
                 .query_builder<movement::position, const repulsion_radius>()
                 .with<repulsion_radius>()
                 .build();
    flecs::entity e1 = it.entity(i);

    q.each([&](flecs::entity e2, movement::position& p2, const repulsion_radius rr2) {
        if (e1 < e2) {
            float dx = p1.x - p2.x;
            float dy = p1.y - p2.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < rr1.radius + rr2.radius) {
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
    const movement::position& p1,
    const interaction_radius& ir1
) {
    auto q = it.world().query_builder<const movement::position, const interaction_radius>().build();
    flecs::entity e1 = it.entity(i);

    q.each([&](flecs::entity e2, const movement::position& p2, const interaction_radius& ir2) {
        if (e1 != e2) {
            float dx = p1.x - p2.x;
            float dy = p1.y - p2.y;
            float distance = std::sqrt(dx * dx + dy * dy);

            if (distance < ir1.radius + ir2.radius) {
                e1.add<interaction_tag>(e2);
            } else {
                e1.remove<interaction_tag>(e2);
            }
        }
    });
}

void physical_interaction::init(flecs::world& world) {
    init_components<repulsion_radius, interaction_tag, interaction_radius>(world);

    world.system<const movement::position, const interaction_radius>("InteractionSystem")
        .kind(flecs::PreUpdate)
        .each(interaction_handle_system);

    world.system<movement::position, const repulsion_radius>("RepulsionSystem")
        .kind(flecs::PreUpdate)
        .each(repulsion_system);
}
